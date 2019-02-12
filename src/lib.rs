#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/wat.rs"));

#[macro_use] extern crate lazy_static;
extern crate regex;

use regex::Regex;
use std::ffi::{CStr,CString};

pub unsafe extern "C" fn sign_event_callback(event: *mut sign_change_event) {
    lazy_static! {
        static ref REGEX: Regex = Regex::new("(?i)&([0-9a-fklmnor&])").unwrap();
    }
    let lines = *(*event).lines;
    let length = lines.length;
    let array = lines.array;

    for i in 0..length {
        let mut c_buf = array.offset(i as isize).read();
        let c_str = CStr::from_ptr(c_buf);
        let line = c_str.to_str().unwrap();
        if line.is_empty() {
            continue
        }
        let replaced = REGEX.replace_all(line, "§$1").replace("§&", "&");

        delete_string(&mut c_buf);

        let result = CString::new(replaced).unwrap();
        let new_result = create_string(result.as_ptr() as *mut _);

        array.offset(i as isize).write(new_result);
    }
}

#[no_mangle]
pub unsafe extern "C" fn wat_init() {
    register_sign_change_callback(Some(sign_event_callback));
}
