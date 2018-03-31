/*
 * Author: Jia Yang
 */

#ifndef JVM_TYPEANNOTATION_H
#define JVM_TYPEANNOTATION_H

//struct TypeAnnotation {
//    u1 target_type;
//    union {
//        struct {
//
//        } type_parameter_target;
//        struct {
//
//        } supertype_target;
//        struct {
//
//        } type_parameter_bound_target;
//        struct {
//
//        } empty_target;
//        struct {
//
//        } method_formal_parameter_target;
//        struct {
//
//        } throws_target;
//        struct {
//
//        } localvar_target;
//        struct {
//
//        } catch_target;
//        struct {
//
//        } offset_target;
//        struct {
//
//        } type_argument_target;
//    } target_info;
//
//    struct {
//        u1 pathLength;  // u1
//
//        struct  {
//            u1 typePathKind;  // u1
//            u1 typeArgumentIndex;  // u1
//        } path;
//    } target_path;
//
//    u2 type_index;
//    u2 num_element_value_pairs;
//    struct {
//        u2 element_name_index;
//        element_value *value;
//    } *element_value_pairs; // [num_element_value_pairs];
//};

#endif //JVM_TYPEANNOTATION_H
