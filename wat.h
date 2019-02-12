#ifndef __WAT_H__
#define __WAT_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Typically defined in includes, but that makes for a messy import
#define bool _Bool
#define int8_t signed char
#define int16_t signed short int
#define int32_t signed int
#define int64_t signed long int

// wat.h
#ifdef _WIN32
    #define WATCALL __cdecl
#else
    #define WATCALL
#endif

#ifdef _WIN32
    #define WATEXPORT __declspec(dllexport)
#else
    #define WATEXPORT __attribute__((visibility("default")))
#endif // _WIN32

#define WAT_INIT wat_init
#define WAT_CLOSE wat_close

#define WAT_CLONE 0
#define WAT_COPY 1

// uuid.h
#define UUID_SIZE 16

typedef struct {
    int8_t bytes[UUID_SIZE];
} uuid;

/**
 * Copy the data from src into dest. dest must not be NULL, false will be returned. src may be NULL, in which case dest
 * will be set to NULL and true will be returned. Note that uuid is treated more like a c-string in that it's not
 * reference counted. Instances of uuid's are destroyed when their containing struct is destroyed, so to retain one you
 * must first copy it and then manage your instance yourself. The length of a UUID is always 16 bytes.
 * @param dest The uuid to copy to, may not be NULL.
 * @param src The uuid to copy from, may be NULL.
 * @return true if and only if the copy finished successfully. Invalid parameters or malloc failures will return false.
 */
WATEXPORT bool WATCALL copy_uuid(uuid **dest, uuid *src);


// event/event_callbacks.h


// logger.h
WATEXPORT void WATCALL bukkit_log_fine(const char *fmt, ...);
WATEXPORT void WATCALL bukkit_log_info(const char *fmt, ...);
WATEXPORT void WATCALL bukkit_log_warning(const char *fmt, ...);
WATEXPORT void WATCALL bukkit_log_severe(const char *fmt, ...);


// util.h
/**
 * Convenience method for `free`ing C-strings created by wat. You should use this rather than calling `free` yourself or
 * using your language to delete these strings, as your allocator may not be the same. Similarly, whenever passing a
 * string to wat, use create_string(char *string) to create a string with the same text that wat can handle.
 * @param[in] string A pointer to the string to delete
 */
WATEXPORT void WATCALL delete_string(char **string);

/**
 * When passing new strings to wat, you should use this function to create strings that wat can handle. This will take
 * the C-string passed to it and make a copy using wat's allocator. When using this function, you should create your
 * string, call this function, then `free` or otherwise delete your string and pass the string you retrieve from this
 * function to wat instead.
 * @param[in] string The string to copy
 * @return The new string, created using wat's allocator
 */
WATEXPORT char * WATCALL create_string(char *string);


// struct/arrays.h
// player_array
struct player_array {
    int ref_count;
    int length;
    int alloc;
    struct player **array;
};

/**
 * Delete the given player_array and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a player_array.
 * @param[in] s The player_array to delete.
 */
WATEXPORT void WATCALL delete_player_array(struct player_array **s);

/**
 * Clone the given player_array and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The player_array to clone.
 * @return The same player_array, with the ref_count field incremented.
 */
WATEXPORT struct player_array * WATCALL clone_player_array(struct player_array *s);

/**
 * Deep copy the src player_array to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_player_array(struct player_array **dest, struct player_array *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single player_array pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new player_array). This function will set the value of src to the value
 * provided by newval and call delete_player_array on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_player_array(struct player_array **src, struct player_array *newval);

/**
 * Add a player_array to the end of the given array. The array will be resized if necessary. s must not be NULL, false will
 * be returned. entry may be NULL, in which case nothing will happen and true will be returned. false is returned for
 * all other cases where the add fails, for example if the resize fails. type should either be WAT_CLONE or WAT_COPY.
 * @param[in] s The array to add the player_array to, must not be NULL.
 * @param[in] entry The player_array to add to the array, may be NULL.
 * @param[in] type Either WAT_CLONE or WAT_COPY to determine how references are managed.
 * @return true if and only if the add succeeds, false in all other cases.
 */
WATEXPORT bool WATCALL add_player_array_entry(struct player_array *s, struct player *entry, int type);
// chunk_array
struct chunk_array {
    int ref_count;
    int length;
    int alloc;
    struct chunk **array;
};

/**
 * Delete the given chunk_array and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a chunk_array.
 * @param[in] s The chunk_array to delete.
 */
WATEXPORT void WATCALL delete_chunk_array(struct chunk_array **s);

/**
 * Clone the given chunk_array and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The chunk_array to clone.
 * @return The same chunk_array, with the ref_count field incremented.
 */
WATEXPORT struct chunk_array * WATCALL clone_chunk_array(struct chunk_array *s);

/**
 * Deep copy the src chunk_array to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_chunk_array(struct chunk_array **dest, struct chunk_array *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single chunk_array pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new chunk_array). This function will set the value of src to the value
 * provided by newval and call delete_chunk_array on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_chunk_array(struct chunk_array **src, struct chunk_array *newval);

/**
 * Add a chunk_array to the end of the given array. The array will be resized if necessary. s must not be NULL, false will
 * be returned. entry may be NULL, in which case nothing will happen and true will be returned. false is returned for
 * all other cases where the add fails, for example if the resize fails. type should either be WAT_CLONE or WAT_COPY.
 * @param[in] s The array to add the chunk_array to, must not be NULL.
 * @param[in] entry The chunk_array to add to the array, may be NULL.
 * @param[in] type Either WAT_CLONE or WAT_COPY to determine how references are managed.
 * @return true if and only if the add succeeds, false in all other cases.
 */
WATEXPORT bool WATCALL add_chunk_array_entry(struct chunk_array *s, struct chunk *entry, int type);
// string_array
struct string_array {
    int ref_count;
    int length;
    int alloc;
    char **array;
};

/**
 * Delete the given string_array and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a string_array.
 * @param[in] s The string_array to delete.
 */
WATEXPORT void WATCALL delete_string_array(struct string_array **s);

/**
 * Clone the given string_array and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The string_array to clone.
 * @return The same string_array, with the ref_count field incremented.
 */
WATEXPORT struct string_array * WATCALL clone_string_array(struct string_array *s);

/**
 * Deep copy the src string_array to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_string_array(struct string_array **dest, struct string_array *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single string_array pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new string_array). This function will set the value of src to the value
 * provided by newval and call delete_string_array on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_string_array(struct string_array **src, struct string_array *newval);

/**
 * Add a string_array to the end of the given array. The array will be resized if necessary. s must not be NULL, false will
 * be returned. entry may be NULL, in which case nothing will happen and true will be returned. false is returned for
 * all other cases where the add fails, for example if the resize fails. type should either be WAT_CLONE or WAT_COPY.
 * @param[in] s The array to add the string_array to, must not be NULL.
 * @param[in] entry The string_array to add to the array, may be NULL.
 * @param[in] type Either WAT_CLONE or WAT_COPY to determine how references are managed.
 * @return true if and only if the add succeeds, false in all other cases.
 */
WATEXPORT bool WATCALL add_string_array_entry(struct string_array *s, char *entry, int type);
// entity_array
struct entity_array {
    int ref_count;
    int length;
    int alloc;
    struct entity **array;
};

/**
 * Delete the given entity_array and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a entity_array.
 * @param[in] s The entity_array to delete.
 */
WATEXPORT void WATCALL delete_entity_array(struct entity_array **s);

/**
 * Clone the given entity_array and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The entity_array to clone.
 * @return The same entity_array, with the ref_count field incremented.
 */
WATEXPORT struct entity_array * WATCALL clone_entity_array(struct entity_array *s);

/**
 * Deep copy the src entity_array to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_entity_array(struct entity_array **dest, struct entity_array *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single entity_array pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new entity_array). This function will set the value of src to the value
 * provided by newval and call delete_entity_array on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_entity_array(struct entity_array **src, struct entity_array *newval);

/**
 * Add a entity_array to the end of the given array. The array will be resized if necessary. s must not be NULL, false will
 * be returned. entry may be NULL, in which case nothing will happen and true will be returned. false is returned for
 * all other cases where the add fails, for example if the resize fails. type should either be WAT_CLONE or WAT_COPY.
 * @param[in] s The array to add the entity_array to, must not be NULL.
 * @param[in] entry The entity_array to add to the array, may be NULL.
 * @param[in] type Either WAT_CLONE or WAT_COPY to determine how references are managed.
 * @return true if and only if the add succeeds, false in all other cases.
 */
WATEXPORT bool WATCALL add_entity_array_entry(struct entity_array *s, struct entity *entry, int type);


// struct/vector.h
// vector
struct vector {
    int ref_count;
    double x;
    double y;
    double z;
    int32_t block_x;
    int32_t block_y;
    int32_t block_z;
};

/**
 * Delete the given vector and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a vector.
 * @param[in] s The vector to delete.
 */
WATEXPORT void WATCALL delete_vector(struct vector **s);

/**
 * Clone the given vector and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The vector to clone.
 * @return The same vector, with the ref_count field incremented.
 */
WATEXPORT struct vector * WATCALL clone_vector(struct vector *s);

/**
 * Deep copy the src vector to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_vector(struct vector **dest, struct vector *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single vector pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new vector). This function will set the value of src to the value
 * provided by newval and call delete_vector on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_vector(struct vector **src, struct vector *newval);


// marshal.h


// struct/location.h
// location
struct location {
    int ref_count;
    uuid *world_id;
    double x;
    double y;
    double z;
    float pitch;
    float yaw;
};

/**
 * Delete the given location and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a location.
 * @param[in] s The location to delete.
 */
WATEXPORT void WATCALL delete_location(struct location **s);

/**
 * Clone the given location and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The location to clone.
 * @return The same location, with the ref_count field incremented.
 */
WATEXPORT struct location * WATCALL clone_location(struct location *s);

/**
 * Deep copy the src location to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_location(struct location **dest, struct location *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single location pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new location). This function will set the value of src to the value
 * provided by newval and call delete_location on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_location(struct location **src, struct location *newval);


// struct/entity.h
// entity
struct entity {
    int ref_count;
    struct location *location;
    struct vector *velocity;
    double height;
    double width;
    bool on_ground;
    uuid *world_id;
    int32_t entity_id;
    int32_t fire_ticks;
    int32_t max_fire_ticks;
    bool is_dead;
    bool is_valid;
    bool is_empty;
    float fall_distance;
    uuid *unique_id;
    int32_t ticks_lived;
    char *entity_type;
    bool inside_vehicle;
    uuid *vehicle_id;
    bool custom_name_visible;
    bool glowing;
    bool invulnerable;
    bool silent;
    bool gravity;
    int32_t portal_cooldown;
    struct string_array *scoreboard_tags;
    char *piston_move_reaction;
};

/**
 * Delete the given entity and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a entity.
 * @param[in] s The entity to delete.
 */
WATEXPORT void WATCALL delete_entity(struct entity **s);

/**
 * Clone the given entity and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The entity to clone.
 * @return The same entity, with the ref_count field incremented.
 */
WATEXPORT struct entity * WATCALL clone_entity(struct entity *s);

/**
 * Deep copy the src entity to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_entity(struct entity **dest, struct entity *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single entity pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new entity). This function will set the value of src to the value
 * provided by newval and call delete_entity on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_entity(struct entity **src, struct entity *newval);


// struct/player.h
// player
struct player {
    int ref_count;
    char *display_name;
    char *player_list_name;
    struct location *compass_target;
    bool sneaking;
    bool sprinting;
    bool sleeping_ignored;
    int32_t total_experience;
    int32_t level;
    float saturation;
    int32_t food_level;
    struct location *bed_spawn_location;
    uuid *uuid;
};

/**
 * Delete the given player and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a player.
 * @param[in] s The player to delete.
 */
WATEXPORT void WATCALL delete_player(struct player **s);

/**
 * Clone the given player and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The player to clone.
 * @return The same player, with the ref_count field incremented.
 */
WATEXPORT struct player * WATCALL clone_player(struct player *s);

/**
 * Deep copy the src player to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_player(struct player **dest, struct player *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single player pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new player). This function will set the value of src to the value
 * provided by newval and call delete_player on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_player(struct player **src, struct player *newval);


// struct/sign_change_event.h
// sign_change_event
struct sign_change_event {
    int ref_count;
    struct player *player;
    struct string_array *lines;
    bool cancel;
};

/**
 * Delete the given sign_change_event and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a sign_change_event.
 * @param[in] s The sign_change_event to delete.
 */
WATEXPORT void WATCALL delete_sign_change_event(struct sign_change_event **s);

/**
 * Clone the given sign_change_event and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The sign_change_event to clone.
 * @return The same sign_change_event, with the ref_count field incremented.
 */
WATEXPORT struct sign_change_event * WATCALL clone_sign_change_event(struct sign_change_event *s);

/**
 * Deep copy the src sign_change_event to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_sign_change_event(struct sign_change_event **dest, struct sign_change_event *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single sign_change_event pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new sign_change_event). This function will set the value of src to the value
 * provided by newval and call delete_sign_change_event on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_sign_change_event(struct sign_change_event **src, struct sign_change_event *newval);


// struct/chunk.h
// chunk
struct chunk {
    int ref_count;
    int32_t x;
    int32_t z;
    uuid *world_id;
    struct entity_array *entities;
    bool loaded;
    bool slime_chunk;
};

/**
 * Delete the given chunk and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a chunk.
 * @param[in] s The chunk to delete.
 */
WATEXPORT void WATCALL delete_chunk(struct chunk **s);

/**
 * Clone the given chunk and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The chunk to clone.
 * @return The same chunk, with the ref_count field incremented.
 */
WATEXPORT struct chunk * WATCALL clone_chunk(struct chunk *s);

/**
 * Deep copy the src chunk to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_chunk(struct chunk **dest, struct chunk *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single chunk pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new chunk). This function will set the value of src to the value
 * provided by newval and call delete_chunk on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_chunk(struct chunk **src, struct chunk *newval);


// struct/world.h
// world
struct world {
    int ref_count;
    struct chunk_array *loaded_chunks;
    struct entity_array *entities;
    struct entity_array *living_entities;
    struct player_array *players;
    char *name;
    uuid *world_id;
    struct location *spawn_location;
    int64_t time;
    int64_t full_time;
    bool has_storm;
    int32_t weather_duration;
    bool thundering;
    int32_t thunder_duration;
    int64_t seed;
    bool pvp;
    bool allow_animals;
    bool allow_monsters;
    int32_t max_height;
    int32_t sea_level;
    bool keep_spawn_in_memory;
    bool auto_save;
    char *world_type;
    bool can_generate_structures;
    int64_t ticks_per_animal_spawns;
    int64_t ticks_per_monster_spawns;
    int32_t monster_spawn_limit;
    int32_t animal_spawn_limit;
    int32_t water_animal_spawn_limit;
    int32_t ambient_spawn_limit;
    struct string_array *game_rules;
};

/**
 * Delete the given world and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a world.
 * @param[in] s The world to delete.
 */
WATEXPORT void WATCALL delete_world(struct world **s);

/**
 * Clone the given world and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The world to clone.
 * @return The same world, with the ref_count field incremented.
 */
WATEXPORT struct world * WATCALL clone_world(struct world *s);

/**
 * Deep copy the src world to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_world(struct world **dest, struct world *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single world pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new world). This function will set the value of src to the value
 * provided by newval and call delete_world on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_world(struct world **src, struct world *newval);


// event/sign_change_event.h
typedef void (WATCALL *sign_change_callback)(struct sign_change_event *event);

WATEXPORT void WATCALL register_sign_change_callback(sign_change_callback callback);


// struct/block.h
// block
struct block {
    int ref_count;
    char *type;
    int8_t light_level;
    int8_t light_from_sky;
    int8_t light_from_blocks;
    struct location *location;
    struct chunk *chunk;
};

/**
 * Delete the given block and set the pointer value to NULL. The pointer given will always be set to NULL
 * after calling this function. It is fine to pass NULL to this function, in which case nothing will happen. This function
 * will decrement the ref_count field and only delete it once it reaches 0. You should call this function as soon as you
 * are finished with a block.
 * @param[in] s The block to delete.
 */
WATEXPORT void WATCALL delete_block(struct block **s);

/**
 * Clone the given block and return it back to the caller. This function always returns the same pointer provided.
 * This function increments the ref_count field by 1 before returning. Passing NULL to this function will result in NULL
 * being returned.
 * @param[in] s The block to clone.
 * @return The same block, with the ref_count field incremented.
 */
WATEXPORT struct block * WATCALL clone_block(struct block *s);

/**
 * Deep copy the src block to dest. dest will be a deep copy of src (any containing structs will also be deep copied)
 * and its ref_count will be 1 (which is true for all children as well). src's ref_count is not modified. dest must not
 * be NULL. src may be NULL, in which case nothing is copied and dest is set to NULL. This function will return true if
 * the copy succeeds, and false if otherwise (for example, if malloc returns a NULL pointer).
 * @param[out] dest The destination to copy to. Must not be NULL.
 * @param[in] src The source to copy from. May be NULL.
 * @return true if an only if the copy succeeded.
 */
WATEXPORT bool WATCALL copy_block(struct block **dest, struct block *src);

/**
 * Reassign a single pointer src to a new value. This is useful if you have a single block pointer and want to do
 * some operation with it that consumes it (after the call it needs to be deleted) and the pointer reassigned to a new
 * value (the result of a function that returns a new block). This function will set the value of src to the value
 * provided by newval and call delete_block on the previous value of src.
 * @param[in,out] src The value to delete and reassign with newval. Must not be NULL.
 * @param[in] newval The value to assign to src. May be NULL.
 */
WATEXPORT void WATCALL reassign_block(struct block **src, struct block *newval);


// event/player_join_event.h
struct player_join_event {
    struct player *player;
    const char *join_message;
};

typedef void (WATCALL *player_join_callback)(struct player_join_event *event);

WATEXPORT void WATCALL register_player_join_callback(player_join_callback callback);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __WAT_H__
