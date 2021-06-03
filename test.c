#include <stdio.h>
#include "src/cabin.h"
#include "src/hash.h"
#include "src/jni.h"
#include "src/meta.h"
#include "src/object.h"
#include "src/sysinfo.h"

static JavaVM *g_vm;
static JNIEnv *g_jni_env;

static void test_sys_info()
{
    printf("-------------- test_sys_info ------------------\n");
    printf("processor number: %d\n", processor_number());
    printf("page size: %d\n", page_size());
    printf("os name: %s\n", os_name());
    printf("os arch: %s\n", os_arch());
    printf("is big endian?: %d\n", is_big_endian());
}

static void test_load_class()
{
    printf("-------------- test_load_class ------------------\n");
    const char *class_names[] = {
        "boolean", "java/lang/Class"
    };

    for (int i = 0; i < ARRAY_LENGTH(class_names); i++) {
        Class *c = load_boot_class(class_names[i]);
        printf("%p, %s, %s, %d\n", c, c->class_name, 
                        c->java_mirror->jvm_mirror->class_name, 
                        c == c->java_mirror->jvm_mirror);
    }
}

static void test_class_loader()
{
    printf("-------------- test_class_loader ------------------\n");
    PHS *loaders = get_all_class_loaders();
    printf("loaders count: %d\n", loaders->phm.size);
    PHS_TRAVERSAL(loaders, jref, x, {
        if (x == BOOT_CLASS_LOADER)
            printf("boot class loader\n");
        else
            printf("%s\n", x->clazz->class_name);
    });

    PHM *boot_classes = get_all_boot_classes();
    printf("---\n\tboot classes count: %d\n", boot_classes->size);
    PHM_TRAVERSAL(boot_classes, Class *, c, {
        printf("\t\t%s\n", c->class_name);
    });
}

static const char *method_descriptors[] = {
        "()V",
        "(I)V",
        "(B)C",
        "(Ljava/lang/Integer;)V",
        "(Ljava/lang/Object;[[BLjava/lang/Integer;[Ljava/lang/Object;)V",
        "(II[Ljava/lang/String;)Ljava/lang/Integer;",
        "([Ljava/io/File;)Ljava/lang/Object;",
        "([[[Ljava/lang/Double;)[[Ljava/lang/Object;",
        "(ZBSIJFD)[[Ljava/lang/String;",
        "(ZZZZZZZZZZZZZZZZ)Z",
};

static void test_method_descriptor()
{
    printf("-------------- test_method_descriptor ------------------\n");

#define PRINT_PTYPES \
    for (int i = 0; i < ptypes->arr_len; i++) { \
        jref o = array_get(jref, ptypes, i); \
        printf("%s, ", o->jvm_mirror->class_name); \
    }

    for (int i = 0; i < ARRAY_LENGTH(method_descriptors); i++) {
        const char *d = method_descriptors[i];
        printf("%s\n\tparameters number: %d\n\tparameter types: ", d, num_elements_in_method_descriptor(d));

        jarrRef ptypes;
        jref rtype;
        parse_method_descriptor(d, BOOT_CLASS_LOADER, &ptypes, &rtype);
        PRINT_PTYPES
        printf("\n\treturn type: %s\n", rtype->jvm_mirror->class_name);
        printf("\tunparse: %s\n\t-----\n\tparameter types: ", unparse_method_descriptor(ptypes, rtype));

        jref mt = findMethodType(d, BOOT_CLASS_LOADER);
        ptypes = get_ref_field(mt, "ptypes", S(array_java_lang_Class));
        PRINT_PTYPES

        printf("\n\tunparse: %s\n", unparse_method_descriptor0(mt));
    }


    printf("---\nunparse(NULL, NULL): %s\n", unparse_method_descriptor(NULL, NULL));
}

static void test_inject_field()
{
    printf("-------------- test_inject_field ------------------\n");
    const char *class_names[] = {
        "java/lang/Object", 
        "java/lang/Class", 
        "java/lang/Object", // 第二次注入 java/lang/Object
    };

    for (int i = 0; i < ARRAY_LENGTH(class_names); i++) {
        Class *c = load_boot_class(class_names[i]);
        printf("%s\n", c->class_name);

        // 因为 inject_inst_field 只能在 loaded 之后进行，
        // 所以这里为了测试强制设置一下。
        ClassState state = c->state;
        c->state = CLASS_LOADED;
        bool b1 = inject_inst_field(c, "inject1", "C");
        bool b2 = inject_inst_field(c, "inject2", "I");
        bool b3 = inject_inst_field(c, "inject3", "J");
        c->state = state;
        
        printf("\t%d, %d, %d\n", b1, b2, b3);
        printf("\t%s\n", get_class_info(c));
    }
}

static void init_int_array(jarrRef arr, jint dim, const jint *lens)
{
    if (dim <= 0)
        return;

    jint arr_len = lens[0];

    if (dim == 1) {
        for (jint i = 0; i < arr_len; i++)
            array_set_int(arr, i, i + 1);
        return;
    }

    for (jint i = 0; i < arr_len; i++) {
        init_int_array(array_get(jarrRef, arr, i), dim - 1, lens + 1);
    }
}

static void print_int_array(jarrRef arr, jint dim, const jint *lens)
{
    if (dim <= 0)
        return;

    jint arr_len = lens[0];

    if (dim == 1) {
        for (jint i = 0; i < arr_len; i++) {
            printf("%d, ", array_get(jint, arr, i));
        }
        printf("\n");
        return;
    }

    for (jint i = 0; i < arr_len; i++) {
        print_int_array(array_get(jarrRef, arr, i), dim - 1, lens + 1);
    }
}

static void test_new_array()
{
    printf("-------------- test_new_array ------------------\n");
    jarrRef arr = alloc_array0(BOOT_CLASS_LOADER, "[I", 10);
    print_int_array(arr, 1, (jint[]){10}); // should be all 0

    init_int_array(arr, 1, (jint[]){10});
    print_int_array(arr, 1, (jint[]){10});
}

static void test_new_multi_array()
{
    printf("-------------- test_new_multi_array ------------------\n");
    Class *ac = load_array_class(BOOT_CLASS_LOADER, "[[[I");
    jint dim = 3;
    jint lens[] = { 2, 3, 4 };
    jarrRef arr = alloc_multi_array(ac, dim, lens);
    print_int_array(arr, dim, lens); // should be all 0

    init_int_array(arr, dim, lens);
    print_int_array(arr, dim, lens);
}

int main()
{
    JNI_CreateJavaVM(&g_vm, (void **) &g_jni_env, NULL);

    test_sys_info();
    test_load_class();
    test_class_loader();
    test_method_descriptor();
    test_inject_field();
    test_new_array();
    test_new_multi_array();

    return 0;
}