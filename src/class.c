#include <assert.h>
#include "cabin.h"
#include "dynstr.h"
#include "constants.h"
#include "attributes.h"


// 计算字段的个数，同时给它们编号
static void calc_fields_id(Class *c)
{
    assert(c != NULL);

    int ins_id = 0;
    if (c->super_class != NULL) {
        ins_id = c->super_class->inst_fields_count;
    }

    // for (Field *f: fields) {
    //     if (!f->isStatic()) {
    //         f->id = ins_id++;
    //         if (f->category_two)
    //             ins_id++;
    //     }
    // }
    for (int i = 0; i < c->fields_count; i++) {
        Field *f = c->fields + i;
        if (!IS_STATIC(f)) {
            f->id = ins_id++;
            if (f->category_two)
                ins_id++;
        }
    }

    c->inst_fields_count = ins_id;
}

static void parse_attribute(Class *c, BytecodeReader *r)
{
    assert(c != NULL && r != NULL);
    u2 attr_count = bcr_readu2(r);

    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = cp_utf8(&c->cp, bcr_readu2(r));
        u4 attr_len = bcr_readu4(r);

        if (S(Signature) == attr_name) {
            c->signature = cp_utf8(&c->cp, bcr_readu2(r));
        } else if (S(Synthetic) == attr_name) {
            SET_SYNTHETIC(c);
        } else if (S(Deprecated) == attr_name) {
            c->deprecated = true;
        } else if (S(SourceFile) == attr_name) {
            u2 source_file_index = bcr_readu2(r);
            if (source_file_index > 0) {
                c->source_file_name = cp_utf8(&c->cp, source_file_index);
            }
        } else if (S(EnclosingMethod) == attr_name) {
            u2 class_index = bcr_readu2(r); // 指向 CONSTANT_Class_info
            u2 method_index = bcr_readu2(r); // 指向 CONSTANT_NameAndType_info

            if (class_index > 0) {
                c->enclosing.clazz = resolve_class(&c->cp, class_index);

                if (method_index > 0) {
                    c->enclosing.name = alloc_string(cp_name_of_name_and_type(&c->cp, method_index));
                    c->enclosing.descriptor = alloc_string(cp_type_of_name_and_type(&c->cp, method_index));
                }
            }
        } else if (S(BootstrapMethods) == attr_name) {
            c->bootstrap_methods_count = bcr_readu2(r);
            BUILD_ARRAY(c->bootstrap_methods, c->bootstrap_methods_count, bootstrap_method_init, r);
        } else if (S(InnerClasses) == attr_name) {
            c->inner_classes_count = bcr_readu2(r);
            BUILD_ARRAY(c->inner_classes, c->inner_classes_count, inner_class_init, r);
        } else if (S(SourceDebugExtension) == attr_name) { // ignore
//            u1 source_debug_extension[attr_len];
//            bcr_read_bytes(reader, source_debug_extension, attr_len);
            bcr_skip(r, attr_len); // todo
        } else if (S(RuntimeVisibleAnnotations) == attr_name) {
            c->rt_visi_annos_count = bcr_readu2(r);
            BUILD_ARRAY(c->rt_visi_annos, c->rt_visi_annos_count, annotation_init, r);    
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) {
            c->rt_invisi_annos_count = bcr_readu2(r);
            BUILD_ARRAY(c->rt_invisi_annos, c->rt_invisi_annos_count, annotation_init, r);
        } else if (S(Module) == attr_name) {
            c->module_info.module = module_create(&c->cp, r);
        } else if (S(ModulePackages) == attr_name) {
            u2 num = c->module_info.module_packages_count = bcr_readu2(r);
            c->module_info.module_packages = (const utf8_t **) vm_malloc(num * sizeof(*(c->module_info.module_packages)));
            for (int j = 0; j < num; j++) {
                u2 index = bcr_readu2(r);
                c->module_info.module_packages[j] = cp_package_name(&c->cp, index);
            }
        } else if (S(ModuleMainClass) == attr_name) {
            c->module_info.module_main_class = cp_class_name(&c->cp, bcr_readu2(r));
        } else if (S(NestHost) == attr_name) {
            c->nest_host = resolve_class(&c->cp, bcr_reads2(r));
        } else if (S(NestMembers) == attr_name) {
            u2 num = c->nest_members_count = bcr_readu2(r);
            c->nest_members = vm_malloc(num * sizeof(*c->nest_members));
            for (u2 j = 0; j < num; j++) {
                c->nest_members[j].loaded = false;
                c->nest_members[j].class_name = cp_class_name(&c->cp, bcr_readu2(r));
                // todo 不要在这里 load_class，有死循环的问题。
                // 比如java.lang.invoke.TypeDescriptor和其NestMember：java.lang.invoke.TypeDescriptor$OfField
//                nest_members.push_back(load_class(loader, name));
            }
        } else { // unknown attribute
            printvm("unknown attribute: %s\n", attr_name); // todo
            bcr_skip(r, attr_len);
        }
    }

    // A class with no NestHost or NestMembers attribute,
    // implicitly forms a nest with itself as the nest host, and sole nest member.
    if (c->nest_host == NULL)
        c->nest_host = c;
    if (c->nest_members_count == 0) {
        c->nest_members = vm_malloc(sizeof(*c->nest_members));
        c->nest_members[0].loaded = true;
        c->nest_members[0].c = c;
        c->nest_members_count = 1;
    }
}

void create_vtable()
{

}

void create_itable()
{

}

#if 0
void Class::createVtable()
{
    assert(vtable.empty());

    if (super_class == NULL) {
        int i = 0;
        for (auto &m : methods) {
            if (m->isVirtual()) {
                vtable.push_back(m);
                m->vtable_index = i++;
            }
        }
        return;
    }

    // 将父类的vtable复制过来
    vtable.assign(super_class->vtable.begin(), super_class->vtable.end());

    for (auto m : methods) {
        if (m->isVirtual()) {
            auto iter = find_if(vtable.begin(), vtable.end(), [=](Method *m0){
                return utf8::equals(m->name, m0->name) && utf8::equals(m->descriptor, m0->descriptor); });
            if (iter != vtable.end()) {
                // 重写了父类的方法，更新
                m->vtable_index = (*iter)->vtable_index;
                *iter = m;
            } else {
                // 子类定义了要给新方法，加到 vtable 后面
                vtable.push_back(m);
                m->vtable_index = vtable.size() - 1;
            }
        }
    }
}

Class::ITable::ITable(const Class::ITable &itable)
{
    interfaces.assign(itable.interfaces.begin(), itable.interfaces.end());
    methods.assign(itable.methods.begin(), itable.methods.end());
}

Class::ITable& Class::ITable::operator=(const Class::ITable &itable)
{
    interfaces.assign(itable.interfaces.begin(), itable.interfaces.end());
    methods.assign(itable.methods.begin(), itable.methods.end());
    return *this;
}

void Class::ITable::add(const ITable &itable)
{
    for (auto ifc: itable.interfaces) {
        interfaces.emplace_back(ifc.first, methods.size());  
        for (auto m: itable.methods) 
            methods.push_back(m);
    }
}

Method *Class::findFromITable(Class *interface_class, int itable_index)
{
    assert(interface_class != NULL && interface_class->isInterface());

    for (auto interface: itable.interfaces) {
        if (interface.first->equals(interface_class)) {
            size_t offset = interface.second;
            assert(offset + itable_index < itable.methods.size());
            return itable.methods[offset + itable_index];
        }
    }

    return NULL;
}

/*
 * 为什么需要itable,而不是用vtable解决所有问题？
 * 一个类可以实现多个接口，而每个接口的函数编号是个自己相关的，
 * vtable 无法解决多个对应接口的函数编号问题。
 * 而对继承一个类只能继承一个父亲，子类只要包含父类vtable，
 * 并且和父类的函数包含部分编号是一致的，就可以直接使用父类的函数编号找到对应的子类实现函数。
 */
void Class::createItable()
{
    if (isInterface()) {
        // 接口间的继承虽然用 extends 关键字（可以同时继承多个接口），但被继承的接口不是子接口的 super_class，
        // 而是在子接口的 interfaces 里面。所以接口的 super_class 就是 java/lang/Object 

        for (Class *super_interface: interfaces) {
            itable.add(super_interface->itable);
        }

        itable.interfaces.emplace_back(this, itable.methods.size());
        int index = 0;
        for (Method *m : methods) {
            // todo default 方法怎么处理？进不进 itable？
            // todo 调用 default 方法 生成什么调用指令？
            m->itable_index = index++;
            itable.methods.push_back(m);
        }
        
        return;
    }

    /* parse non interface class */

    if (super_class != NULL) {
        itable = super_class->itable;
    }

    for (auto ifc : interfaces) {
        for (auto tmp : itable.interfaces) {
            if (ifc->equals(tmp.first)) {
                // 此接口已经在 itable.interfaces 中了
                goto next;
            }
        }

        // 发现一个新实现的接口
        itable.interfaces.emplace_back(ifc, itable.methods.size());
        // 检查新实现接口的方法是不是已经被重写了
        for (auto m: ifc->methods) {
            for (auto m0: itable.methods) {
                if (utf8::equals(m->name, m0->name) && utf8::equals(m->descriptor, m0->descriptor)) {
                    m = m0; // 重写了接口方法，更新
                    break;
                }
            }
            itable.methods.push_back(m);
        }
next:;
    }
    
    // 遍历 itable.methods，检查有没有接口函数在本类中被重写了。
    for (size_t i = 0; i < itable.methods.size(); i++) {
        auto m = itable.methods[i];
        for (auto m0 : methods) {
            if (utf8::equals(m->name, m0->name) && utf8::equals(m->descriptor, m0->descriptor)) {
                //m0->itable_index = m->itable_index;
                itable.methods[i] = m0; // 重写了接口方法，更新
                break;
            }
        }
    }
}

#endif

// static void gen_indep_interfaces(Class *c)
// {
//     if (c->super_class != NULL) {
//         c->indep_interfaces = c->super_class->indep_interfaces;
//     }
//     for (u2 i = 0; i < c->interfaces_count; i++) {
//         c->indep_interfaces.insert(c->interfaces[i]);
//     }

//     /* 独立化 indep_interfaces */

//     vector<Class *> need_del_interfaces;
//     for (Class *itf: c->indep_interfaces) {
//         // 将 c 的父接口全部放入
//         for (u2 i = 0; i < itf->interfaces_count; i++) {
//             need_del_interfaces.push_back(itf->interfaces[i]);
//         }
//         // for (Class *itf0: itf->interfaces) {
//         //     need_del_interfaces.push_back(itf0);
//         // }
//     }

//     for (size_t i = 0; i < need_del_interfaces.size(); i++) {
//         Class *del = need_del_interfaces[i];
//         c->indep_interfaces.erase(del);
//         // 将 del 的父接口全部放入
//         for (u2 i = 0; i < del->interfaces_count; i++) {
//             need_del_interfaces.push_back(del->interfaces[i]);
//         }
//         // for (Class *itf: del->interfaces) {
//         //     need_del_interfaces.push_back(itf);
//         // }
//     }
// }

// 根据类名生成包名
static void gen_pkg_name(Class *c)
{
    char *pkg = utf8_dup(c->class_name);
    char *p = strrchr(pkg, '/');
    if (p == NULL) {
        free(pkg);
        c->pkg_name = ""; // 包名可以为空
    } else {
        *p = 0; // 得到包名
        slash_to_dot(pkg);
        const utf8_t *hashed = find_utf8(pkg);
        if (hashed != NULL) {
            free(pkg);
            c->pkg_name = hashed;
        } else {
            c->pkg_name = pkg;
            save_utf8(c->pkg_name);
        }
    }
}

static void preinit_class(Class *c)
{
    assert(c != NULL);

    pthread_mutex_init(&c->clinit_mutex, NULL);
    pthread_mutex_init(&c->string.str_pool_mutex, &g_pthread_mutexattr_recursive);

    c->state = EMPTY_STATE;
}

Class *define_class(jref class_loader, u1 *bytecode, size_t len)
{
    assert(bytecode != NULL);

    Class *c = vm_calloc(sizeof(*c));
    preinit_class(c);

    c->loader = class_loader;
    // c->bytecode = bytecode;

    BytecodeReader r;
    //(bytecode, len);
    bcr_init(&r, bytecode, len);

    u4 magic = bcr_readu4(&r);
    if (magic != 0xcafebabe) {
        // throw java_lang_ClassFormatError("bad magic");
        raise_exception(S(java_lang_ClassFormatError), NULL);  // todo msg
    }

    bcr_readu2(&r); // minor_version
    u2 major_version = bcr_readu2(&r);

    if (major_version != g_classfile_major_version) {
//        thread_throw(new ClassFormatError("bad class version")); // todo
    }

    ConstantPool *cp = &c->cp;
    // init constant pool    
    cp_init(cp, c, bcr_readu2(&r));
    for (u2 i = 1; i < c->cp.size; i++) {
        u1 tag = bcr_readu1(&r);
        cp_set_type(cp, i, tag);
        switch (tag) {
            case JVM_CONSTANT_Class:
            case JVM_CONSTANT_String:
            case JVM_CONSTANT_MethodType:
            case JVM_CONSTANT_Module:
            case JVM_CONSTANT_Package:
                cp_set(cp, i, bcr_readu2(&r));
                break;
            case JVM_CONSTANT_NameAndType:
            case JVM_CONSTANT_Fieldref:
            case JVM_CONSTANT_Methodref:
            case JVM_CONSTANT_InterfaceMethodref:
            case JVM_CONSTANT_Dynamic:
            case JVM_CONSTANT_InvokeDynamic: {
                slot_t index1 = bcr_readu2(&r);
                slot_t index2 = bcr_readu2(&r);
                cp_set(&c->cp, i, (index2 << 16) + index1);
                break;
            }
            case JVM_CONSTANT_Integer: {
                u1 bytes[4];
                bcr_read_bytes(&r, bytes, 4);
                cp_set_int(cp, i, bytes_to_int32(bytes));
                break;
            }
            case JVM_CONSTANT_Float: {
                u1 bytes[4];
                bcr_read_bytes(&r, bytes, 4);
                cp_set_float(cp, i, bytes_to_float(bytes));
                break;
            }
            case JVM_CONSTANT_Long: {
                u1 bytes[8];
                bcr_read_bytes(&r, bytes, 8);
                cp_set_long(cp, i, bytes_to_int64(bytes));
                cp_set_type(cp, ++i, JVM_CONSTANT_Placeholder);
                break;
            }
            case JVM_CONSTANT_Double: {
                u1 bytes[8];
                bcr_read_bytes(&r, bytes, 8);
                cp_set_double(cp, i, bytes_to_double(bytes));
                cp_set_type(cp, ++i, JVM_CONSTANT_Placeholder);
                break;
            }
            case JVM_CONSTANT_Utf8: {
                u2 utf8_len = bcr_readu2(&r);
                char buf[utf8_len + 1];
                bcr_read_bytes(&r, (u1 *) buf, utf8_len);
                buf[utf8_len] = 0;

                const char *utf8 = find_utf8(buf);
                if (utf8 == NULL) {
                    utf8 = strdup(buf);
                    utf8 = save_utf8(utf8);
                }
                cp_set(cp, i, (slot_t) utf8);
                break;
            }
            case JVM_CONSTANT_MethodHandle: {
                slot_t index1 = bcr_readu1(&r); // 这里确实是 readu1, reference_kind
                slot_t index2 = bcr_readu2(&r); // reference_index
                cp_set(cp, i, (index2 << 16) + index1);
                break;
            }
            default:
                // throw java_lang_ClassFormatError("bad constant tag: " + to_string(tag));
                raise_exception(S(java_lang_ClassFormatError), NULL);  // todo msg
        }
    }

    c->access_flags = bcr_readu2(&r);
    c->class_name = cp_class_name(cp, bcr_readu2(&r));
    gen_pkg_name(c);

    u2 super = bcr_readu2(&r);
    if (super == 0) { // invalid constant pool reference
        c->super_class = NULL;
    } else {
        if (utf8_equals(c->class_name, S(java_lang_Object))) {
            // throw java_lang_ClassFormatError("Object has super");
            raise_exception(S(java_lang_ClassFormatError), "Object has super");  // todo msg
            return NULL;
        }
        c->super_class = resolve_class(cp, super);
    }

    // parse interfaces
    c->interfaces_count = bcr_readu2(&r);
    if (c->interfaces_count > 0) {
        c->interfaces = vm_malloc(c->interfaces_count * sizeof(*(c->interfaces)));
        for (u2 i = 0; i < c->interfaces_count; i++)
            c->interfaces[i] = resolve_class(cp, bcr_readu2(&r));
    }

    // parse fields
    c->fields_count = bcr_readu2(&r);
    if (c->fields_count > 0) {
        c->fields = vm_malloc(c->fields_count * sizeof(Field));
        // fields.resize(fields_count);
        // auto last_field = fields_count - 1;
        for (u2 i = 0; i < c->fields_count; i++) {
            init_field(c->fields + i, c, &r);
            // new (fields + i) Field(this, &r);
            // auto f = new Field(this, &r);
            // // 保证所有的 public fields 放在前面             
            // if (f->isPublic())
            //     fields[public_fields_count++] = f;
            // else
            //     fields[last_field--] = f;
        }
    }

    calc_fields_id(c);

    // parse methods
    c->methods_count = bcr_readu2(&r);
    if (c->methods_count > 0) {
        c->methods = vm_malloc(c->methods_count * sizeof(Method));
        for (u2 i = 0; i <c-> methods_count; i++) {
            init_method(c->methods + i, c, &r);
        }
    }

    parse_attribute(c, &r); // parse class attributes

#if 0
    createVtable(); // todo 接口有没有必要创建 vtable
    createItable();
#endif
    // gen_indep_interfaces(this);

    if (g_class_class != NULL) {
        gen_class_object(c);

        // todo module != NULL

        if (c->module_info.module == NULL && class_loader != NULL) {
            assert(class_loader != NULL);
            // 对于unnamed module的处理
            // public final Module getUnnamedModule();
            Method *m = lookup_inst_method(class_loader->clazz, "getUnnamedModule", "()Ljava/lang/Module;");
            jref mo = exec_java_r(m, (slot_t[]) { rslot(class_loader) });

            // set by VM
            // private transient Module module;
//            Field *f = java_mirror->clazz->lookupInstField("module", "Ljava/lang/Module;");
            set_ref_field(c->java_mirror, "module", "Ljava/lang/Module;", mo);
        }

//        jref mmm = java_mirror->getRefField("module", "Ljava/lang/Module;");
//        printvm("%s, %p\n", class_name, mmm);
    }

    c->state = CLASS_LOADED;
    return c;
}

// 创建 primitive class，这两种类型的类由虚拟机直接生成。
Class *define_prim_type_class(const char *class_name)
{
    assert(class_name != NULL);
    assert(is_prim_class_name(class_name));

    Class *c = vm_calloc(sizeof(*c));
    preinit_class(c);

    c->class_name = utf8_dup(class_name); /* 形参class_name可能非持久，复制一份 */
    c->access_flags = JVM_ACC_PUBLIC;
    c->inited = true;
    c->loader = BOOT_CLASS_LOADER;
    c->super_class = g_object_class;

    cp_init(&c->cp, c, 1); // todo

    c->pkg_name = "";

#if 0
    createVtable();  
    createItable();
#endif
    // gen_indep_interfaces(this);

    if (g_class_class != NULL) {
        gen_class_object(c);
    }

    c->state = CLASS_LOADED;
    return c;
}

// 创建 array class，由虚拟机直接生成。
Class *define_array_class(Object *loader, const char *class_name)
{
    assert(class_name != NULL);
    assert(class_name[0] == '[');

    Class *c = vm_calloc(sizeof(*c));
    preinit_class(c);

    c->class_name = utf8_dup(class_name); /* 形参class_name可能非持久，复制一份 */
    c->access_flags = JVM_ACC_PUBLIC;
    c->inited = true;
    c->loader = loader;
    c->super_class = g_object_class;

    cp_init(&c->cp, c, 1); // todo

    c->pkg_name = "";

    c->interfaces_count = 2;
    c->interfaces = vm_malloc(c->interfaces_count * sizeof(*(c->interfaces)));
    c->interfaces[0] = load_boot_class(S(java_lang_Cloneable));
    c->interfaces[1] = load_boot_class(S(java_io_Serializable));

#if 0
    createVtable();  
    createItable();
#endif
    // gen_indep_interfaces(this);

    if (g_class_class != NULL) {
        gen_class_object(c);
    }

    c->state = CLASS_LOADED;
    return c;
}

void gen_class_object(Class *c)
{
    assert(c != NULL);
    if (c->java_mirror == NULL) {
        assert(g_class_class != NULL);
        // static size_t size = sizeof(ClsObj) + g_class_class->inst_fields_count * sizeof(slot_t);

        // Class Object不在堆上分配，因为此对象无需gc。
        c->java_mirror = create_local_object(g_class_class);
        c->java_mirror->jvm_mirror = c;

        // private final ClassLoader classLoader;
        set_ref_field(c->java_mirror, "classLoader", S(sig_java_lang_ClassLoader), c->loader);
        if (is_array_class(c)) {
            Class *component = component_class(c);
            assert(component != NULL);            
            // private final Class<?> componentType;
            set_ref_field(c->java_mirror, "componentType", S(sig_java_lang_Class), component->java_mirror);
        }
    }
    assert(c->java_mirror != NULL);
}

void destroy_class(Class *c)
{
    assert(c != NULL);

    // todo something else
}

bool class_equals(const Class *c1, const Class *c2) 
{
    if (c1 == c2)
        return true;
    if (c1 == NULL || c2 == NULL)
        return false;
    return (c1->loader == c2->loader) && (utf8_equals(c1->class_name, c2->class_name));
}

size_t non_array_object_size(const Class *c) 
{
    assert(!is_array_class(c));
    return sizeof(Object) + c->inst_fields_count*sizeof(slot_t);
}

size_t array_object_size(Class *c, jint arr_len)
{
    assert(is_array_class(c));
    return sizeof(Object) + get_ele_size(c)*arr_len;
}

Field *lookup_field(Class *c, const utf8_t *name, const utf8_t *descriptor)
{
    Field *f;
    Class *clazz = c;
    do {
        if ((f = get_declared_field0(clazz, name, descriptor)) != NULL)
            return f;
        clazz = clazz->super_class;
    } while (clazz != NULL);

    // for (Class *itf: c->indep_interfaces) {
    //     // 在接口 c 及其父接口中查找
    //     if ((f = lookup_field(itf, name, descriptor)) != NULL)
    //         return f;
    // }
    for (u2 i = 0; i < c->interfaces_count; i++) {
        if ((f = lookup_field(c->interfaces[i], name, descriptor)) != NULL)
            return f;
    }

    return NULL;
}

static Field *get_declared_inst_field_noexcept(const Class *c, int id);

Field *lookup_inst_field(Class *c, int id)
{
    // Field *f = NULL;
    Class *clazz = c;
    // do {
    //     try {
    //         return get_declared_inst_field(clazz, id);
    //     } catch (java_lang_NoSuchFieldError &e) { 
    //         // not found
    //     }
    //     clazz = clazz->super_class;
    // } while (clazz != NULL);
    do {
        Field *f = get_declared_inst_field_noexcept(clazz, id);
        if (f != NULL)
            return f;
        clazz = clazz->super_class;
    } while (clazz != NULL);

    // throw java_lang_NoSuchFieldError(string(c->class_name) + ", id = " + to_string(id));
    raise_exception(S(java_lang_NoSuchFieldError), NULL); // todo msg
    return NULL;
}

Field *lookup_static_field(Class *c, const utf8_t *name, const utf8_t *descriptor)
{
    Field *field = lookup_field(c, name, descriptor);
    if (field != NULL && !IS_STATIC(field)) {
        // throw java_lang_IncompatibleClassChangeError();
        raise_exception(S(java_lang_IncompatibleClassChangeError), NULL); // todo msg  
    }
    return field;
}

Field *lookup_inst_field0(Class *c, const utf8_t *name, const utf8_t *descriptor)
{
    Field *field = lookup_field(c, name, descriptor);
    if (field != NULL && IS_STATIC(field)) {
        // throw java_lang_IncompatibleClassChangeError();
        raise_exception(S(java_lang_IncompatibleClassChangeError), NULL); // todo msg  
    }
    return field;
}

Field *get_declared_field(const Class *c, const char *name) 
{
    for (u2 i = 0; i < c->fields_count; i++) {
        if (utf8_equals(c->fields[i].name, name))
            return c->fields + i;
    }
    return NULL;
}

Field *get_declared_field0(const Class *c, const char *name, const char *descriptor) 
{
    for (u2 i = 0; i < c->fields_count; i++) {
        if (utf8_equals(c->fields[i].name, name) && utf8_equals(c->fields[i].descriptor, descriptor))
            return c->fields + i;
    }
    return NULL;
}

static Field *get_declared_inst_field_noexcept(const Class *c, int id)
{
    for (u2 i = 0; i < c->fields_count; i++) {
        if (!IS_STATIC(c->fields + i) && c->fields[i].id == id)
            return c->fields + i;
    }

    return NULL;
}

Field *get_declared_inst_field(const Class *c, int id)
{
    // for (u2 i = 0; i < c->fields_count; i++) {
    //     if (!IS_STATIC(c->fields + i) && c->fields[i].id == id)
    //         return c->fields + i;
    // }
    Field *f = get_declared_inst_field_noexcept(c, id);
    if (f != NULL)
        return f;

    // throw java_lang_NoSuchFieldError(string(c->class_name) + ", id = " + to_string(id));
    raise_exception(S(java_lang_NoSuchFieldError), NULL); // todo msg
    return NULL;
}

bool inject_inst_field(Class *c, const utf8_t *name, const utf8_t *descriptor)
{
    assert(c != NULL && name != NULL && descriptor != NULL);

    if (c->state != CLASS_LOADED) {
        // todo 只能在loaded之后 inject Field
        // throw runtime_error("state: " + to_string(c->state));
        return false;
    }

    // 首先要确定class中不存在和要注入的field同名的field

    for (u2 i = 0; i < c->fields_count; i++) {
        if (utf8_equals(c->fields[i].name, name)) {
            // throw runtime_error(name); // todo
            return false;
        }
    }

    for (Class *clazz = c->super_class; clazz != NULL; clazz = clazz->super_class) {
        for (u2 i = 0; i < clazz->fields_count; i++) {
            // 在父类查找时只查子类可以看见的field，即非private field
            if (!IS_PRIVATE(clazz->fields + i) && utf8_equals(clazz->fields[i].name, name)) {
                // throw runtime_error(name); // todo
                return false;
            }
        }
    }

    // todo 在接口及其父接口中查找

    c->fields = vm_realloc(c->fields, (++c->fields_count) * sizeof(Field));

    int flags = JVM_ACC_PRIVATE | JVM_ACC_SYNTHETIC;
    Field *f = c->fields + c->fields_count - 1;
    init_field0(f, c, name, descriptor, flags);

    // auto f = new (c->fields + c->fields_count - 1) Field(c, name, descriptor, flags);

    f->id = c->inst_fields_count;

    c->inst_fields_count++;
    if (f->category_two)
        c->inst_fields_count++;

    return true;
}

Method *get_declared_method_noexcept(Class *c, const utf8_t *name, const utf8_t *descriptor)
{
    for (u2 i = 0; i < c->methods_count; i++) {
        Method *m = c->methods + i;
        // if (m->isSignaturePolymorphic()) {
        //     if (strcmp(name, "invoke") == 0&&strcmp(m->name, "invoke") == 0) {
        //         m->isSignaturePolymorphic();
        //         printvm("===== %s, %s, %s\n", m->clazz->class_name, m->name, m->descriptor);
        // }
        if (utf8_equals(m->name, name) && utf8_equals(m->descriptor, descriptor))
            return m;
    }

    return NULL;
}

Method *get_declared_method(Class *c, const utf8_t *name, const utf8_t *descriptor)
{
    for (u2 i = 0; i < c->methods_count; i++) {
        Method *m = c->methods + i;
        // if (m->isSignaturePolymorphic()) {
        //     if (strcmp(name, "invoke") == 0&&strcmp(m->name, "invoke") == 0) {
        //         m->isSignaturePolymorphic();
        //         printvm("===== %s, %s, %s\n", m->clazz->class_name, m->name, m->descriptor);
        // }
        if (utf8_equals(m->name, name) && utf8_equals(m->descriptor, descriptor))
            return m;
    }

    Method *m = get_declared_method_noexcept(c, name, descriptor);
    if (m != NULL)
        return m;

    // throw java_lang_NoSuchMethodError(string(c->class_name) + '~' + name + '~' + descriptor);
    raise_exception(S(java_lang_NoSuchMethodError), NULL); // todo msg
    return NULL;
}

Method *get_declared_static_method(Class *c, const utf8_t *name, const utf8_t *descriptor)
{
    for (u2 i = 0; i < c->methods_count; i++) {
        Method *m = c->methods + i;
        if (IS_STATIC(m) && utf8_equals(m->name, name) && utf8_equals(m->descriptor, descriptor))
            return m;
    }

    // throw java_lang_NoSuchMethodError(string(c->class_name) + '~' + name + '~' + descriptor);
    raise_exception(S(java_lang_NoSuchMethodError), NULL); // todo msg
    return NULL;
}

Method *get_declared_inst_method(Class *c, const utf8_t *name, const utf8_t *descriptor)
{
    for (u2 i = 0; i < c->methods_count; i++) {
        Method *m = c->methods + i;
        if (!IS_STATIC(m) && utf8_equals(m->name, name) && utf8_equals(m->descriptor, descriptor))
            return m;
    }

    // throw java_lang_NoSuchMethodError(string(c->class_name) + '~' + name + '~' + descriptor);
    raise_exception(S(java_lang_NoSuchMethodError), NULL); // todo msg
    return NULL;
}

Method *get_declared_poly_method(Class *c, const utf8_t *name)
{
    assert(c != NULL && name != NULL);

    for (u2 i = 0; i < c->methods_count; i++) {
        Method *m = c->methods + i;
        if (utf8_equals(m->name, name) && is_signature_polymorphic(m))
            return m;
    }
    
    return NULL;
}

// vector<Method *> get_declared_methods(Class *c, const utf8_t *name, bool public_only)
// {
//     assert(c != NULL && name != NULL);
//     vector<Method *> declaredMethods;

//     for (u2 i = 0; i < c->methods_count; i++) {
//         Method *m = c->methods + i;
//         if ((!public_only || IS_PUBLIC(m)) && (utf8_equals(m->name, name)))
//             declaredMethods.push_back(m);
//     }

//     return declaredMethods;
// }

Method **get_declared_methods(Class *c, const utf8_t *name, bool public_only, int *count)
{
    assert(c != NULL && name != NULL && count != NULL);
    
    Method **declared_methods = vm_malloc(sizeof(*declared_methods) * c->methods_count);
    *count = 0;

    for (u2 i = 0; i < c->methods_count; i++) {
        Method *m = c->methods + i;
        if ((!public_only || IS_PUBLIC(m)) && (utf8_equals(m->name, name)))
            declared_methods[*count++] = m;
    }

    return declared_methods;
}

Method *get_constructor(Class *c, const utf8_t *descriptor)
{
    return get_declared_method(c, S(object_init), descriptor);
}

Method *get_constructor0(Class *c, jarrRef parameter_types)
{
    assert(parameter_types != NULL);

//    Class *c = loadBootClass("java/lang/invoke/MethodType");
//
//    // public static MethodType methodType(Class<?> rtype, Class<?>[] ptypes);
//    Method *m = c->getDeclaredStaticMethod(
//            "methodType", "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
//    auto mt = RSLOT(execJavaFunc(m, { loadBootClass(S(void))->java_mirror, parameter_types } ));
//
//    // public String toMethodDescriptorString();
//    m = c->getDeclaredInstMethod("toMethodDescriptorString", "()Ljava/lang/String;");
//    auto s = (RSLOT(execJavaFunc(m, {mt})))->toUtf8();
    char *desc = unparse_method_descriptor(parameter_types, NULL);
    return get_constructor(c, desc);
}

Method **get_constructors(Class *c, bool public_only, int *count)
{
    return get_declared_methods(c, S(object_init), public_only, count);
}


#if 0
Method *Class::lookupMethod(const char *name, const char *descriptor)
{
    Method *method = getDeclaredMethod(name, descriptor, false);
    if (method != NULL) {
        return method;
    }

    // todo 是否应该禁止查找父类的私有方法，因为子类看不见父类的私有方法
    Class *super = super_class;
    while (super != NULL) {
        if ((method = super->getDeclaredMethod(name, descriptor, false)) != NULL)
            return method;
        super = super->super_class;
    }

    for (Class *c: indep_interfaces) {
        // 在接口 c 及其父接口中查找
        if ((method = c->lookupMethod(name, descriptor)) != NULL)
            return method;
    }

    return NULL;
}
#endif

Method *lookup_method(Class *c, const char *name, const char *descriptor)
{
    Method *method;

    // try {
    //     return get_declared_method(c, name, descriptor);
    // } catch (java_lang_NoSuchMethodError &e) { }
    method = get_declared_method_noexcept(c, name, descriptor);
    if (method != NULL)
        return method;

    // todo 是否应该禁止查找父类的私有方法，因为子类看不见父类的私有方法

    // Class *super = c->super_class;
    // while (super != NULL) {
    //     try {
    //         return get_declared_method(super, name, descriptor);
    //     } catch (java_lang_NoSuchMethodError &e) { }
    //     super = super->super_class;
    // }

    Class *super = c->super_class;
    while (super != NULL) {
        method = get_declared_method_noexcept(super, name, descriptor);
        if (method != NULL)
            return method;
        super = super->super_class;
    }

    
    // for (Class *ifc: c->indep_interfaces) {
    //     // 在接口 c 及其父接口中查找
    //     if ((method = lookup_method(ifc, name, descriptor)) != NULL)
    //         return method;
    // }
    for (u2 i = 0; i < c->interfaces_count; i++) {
        if ((method = lookup_method(c->interfaces[i], name, descriptor)) != NULL)
            return method;
    }

    return NULL;
}

Method *lookup_static_method(Class *c, const char *name, const char *descriptor)
{
    Method *m = lookup_method(c, name, descriptor);
    if (m != NULL && !IS_STATIC(m)) {
        // throw java_lang_IncompatibleClassChangeError();
        raise_exception(S(java_lang_IncompatibleClassChangeError), NULL); // todo msg  
    }
    return m;
}

Method *lookup_inst_method(Class *c, const char *name, const char *descriptor)
{
    Method *m = lookup_method(c, name, descriptor);
    // todo m == NULL
    if (m != NULL && IS_STATIC(m)) {
        // throw java_lang_IncompatibleClassChangeError();
        raise_exception(S(java_lang_IncompatibleClassChangeError), NULL); // todo msg  
    }
    return m;
}

bool is_subclass_of(Class *son, Class *father)
{
    assert(father != NULL);
    
    if (son == father)
        return true;

    if (son->super_class != NULL && is_subclass_of(son->super_class, father))
        return true;

    for (u2 i = 0; i < son->interfaces_count; i++) {
        if (is_subclass_of(son->interfaces[i], father))
            return true;
    }

    // array class 特殊处理
    if (is_array_class(son) && is_array_class(father) && array_class_dim(son) == array_class_dim(father)) {
        return is_subclass_of(element_class(son), element_class(father));
    }

    return false;
}

int inherited_depth(const Class *clazz) 
{
    int depth = 0;
    const Class *c = clazz->super_class;
    for (; c != NULL; c = c->super_class) {
        depth++;
    }
    return depth;
}

bool is_array_class(const Class *c) 
{
    return c->class_name[0] == '[';
}

bool is_prim_class(const Class *c) 
{
    return is_prim_class_name(c->class_name);
}

bool is_prim_wrapper_class(const Class *c) 
{
    return is_prim_wrapper_class_name(c->class_name);
}

// bool is_void_class(const Class *c) 
// {
//     return strcmp(c->class_name, "void") == 0;
// }

bool is_prim_array_class(const Class *c) 
{
    if (strlen(c->class_name) != 2 || c->class_name[0] != '[')
        return false;

    return strchr("ZBCSIFJD", c->class_name[1]) != NULL;
}

Class *array_class(const Class *c) 
{
    // todo 判断 c 的维度，jvms数组最大维度为255. ARRAY_MAX_DIMENSIONS
    char buf[strlen(c->class_name) + 8]; // big enough

    // 数组
    if (c->class_name[0] == '[') {
        sprintf(buf, "[%s", c->class_name);
        return load_array_class(c->loader, buf);
    }

    // 基本类型
    // todo 判断class是不是void，如果是void不能创建数组
    const char *tmp = get_prim_array_class_name(c->class_name);
    if (tmp != NULL)
        return load_array_class(c->loader, tmp);

    // 类引用
    sprintf(buf, "[L%s;", c->class_name);
    return load_array_class(c->loader, buf);
}

char *get_class_info(const Class *c)
{
    assert(c != NULL);

    DynStr ds;
    dynstr_init(&ds);

    dynstr_printf(&ds, "class: %s\n", c->class_name);

    dynstr_concat(&ds, "\tdeclared static fields: \n");
    for (u2 i = 0; i < c->fields_count; i++) {
        Field *f = c->fields + i;
        if (IS_STATIC(f)) {
            dynstr_printf(&ds, "\t\t%s~%s\n", f->name, f->descriptor);
        }
    }

    dynstr_concat(&ds, "\tdeclared instance fields: \n");
    for (u2 i = 0; i < c->fields_count; i++) {
        Field *f = c->fields + i;
        if (!IS_STATIC(f)) {
            dynstr_printf(&ds, "\t\t%s~%s | %d\n", f->name, f->descriptor, f->id);
        }
    }

    return ds.buf;
}

int array_class_dim(const Class *c) 
{
    assert(c != NULL);
    int d = 0;
    while (c->class_name[d] == '[') d++;
    return d;
}

size_t get_ele_size(Class *c)
{
    assert(c != NULL && is_array_class(c));

    if (c->array.ele_size == 0) {
        // 判断数组单个元素的大小
        // 除了基本类型的数组外，其他都是引用类型的数组
        // 多维数组是数组的数组，也是引用类型的数组
        char t = c->class_name[1]; // jump '['
        if (t == 'Z') {
            c->array.ele_size = sizeof(jbool);
        } else if (t == 'B') {
            c->array.ele_size = sizeof(jbyte);
        } else if (t == 'C') {
            c->array.ele_size = sizeof(jchar);
        } else if (t == 'S') {
            c->array.ele_size = sizeof(jshort);
        } else if (t == 'I') {
            c->array.ele_size = sizeof(jint);
        } else if (t == 'F') {
            c->array.ele_size = sizeof(jfloat);
        } else if (t == 'J') {
            c->array.ele_size = sizeof(jlong);
        } else if (t == 'D') {
            c->array.ele_size = sizeof(jdouble);
        } else {
            c->array.ele_size = sizeof(jref);
        }
    }

    return c->array.ele_size;
}

Class *component_class(Class *c)
{
    if (c->array.comp_class != NULL)
        return c->array.comp_class;

    const char *comp_name = c->class_name;
    if (*comp_name != '[')
        return NULL; // not a array

    comp_name++; // jump a '['

    // 判断 component's type
    if (*comp_name == '[') {
        c->array.comp_class = load_array_class(c->loader, comp_name);
        return c->array.comp_class;
    }

    const utf8_t *prim_class_name = get_prim_class_name(*comp_name);
    if (prim_class_name != NULL) {  // primitive type
        c->array.comp_class = load_boot_class(prim_class_name);
        return c->array.comp_class;
    }

    // 普通类: Lxx/xx/xx; 型
    comp_name++; // jump 'L'
    size_t last = strlen(comp_name) - 1;
    if (comp_name[last] != ';') {
        raise_exception(S(java_lang_UnknownError), NULL); // todo msg
        // throw java_lang_UnknownError();
    } else {
        char buf[last + 1];
        strncpy(buf, comp_name, last);
        buf[last] = 0;
        c->array.comp_class = load_class(c->loader, buf);
        return c->array.comp_class;
    }

    return NULL;
}

Class *element_class(Class *c)
{
    if (c->array.ele_class != NULL)
        return c->array.ele_class;

    if (!is_array_class(c))
        return NULL;

    Class *curr = c;
    while(true) {
        Class *cc = component_class(curr);
        assert(cc != NULL);
        if (!is_array_class(cc)) {
            c->array.ele_class = cc;
            return c->array.ele_class;
        }
        curr = cc;
    }
}

jstrRef intern_string(jstrRef s)
{
    // scoped_lock lock(str_pool_mutex);
    pthread_mutex_lock(&g_string_class->string.str_pool_mutex);

    assert(s != NULL);
    assert(is_string_object(s));    

    // return either the newly inserted element
    // or the equivalent element already in the set
    Object *interned = (Object *) phs_add(g_string_class->string.str_pool, s);
    // Object *interned = *(g_string_class->str_pool->insert(s).first);

    pthread_mutex_unlock(&g_string_class->string.str_pool_mutex);
    // str_pool_mutex.unlock();
    return interned;
}
