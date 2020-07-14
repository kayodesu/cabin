package reflect;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;

/**
 * Status: Fail
 */
public class GenericTest {
    private static class GenericClass<String> { }

    private static final String str = "abc";
    private static final List<String> list = new ArrayList<>();

    public static void typeParameter() {
        System.out.println(GenericClass.class.getTypeParameters().length); // 1
        System.out.println(GenericClass.class.getTypeParameters()[0].toString()); // String
    }

    public static void fieldGenericType() throws NoSuchFieldException {
        System.out.println(String.class == GenericTest.class.getDeclaredField("str").getGenericType()); // true
        Field listField = GenericTest.class.getDeclaredField("list");
        System.out.println(listField.getName()); // list
        System.out.println(List.class == listField.getType()); // true
        System.out.println(listField.getModifiers()); // 26 /* private static final */
        System.out.println(listField.getGenericType().toString()); // java.util.List<java.lang.String>
    }

    public static void main(String[] args) throws Exception {
        typeParameter();
        fieldGenericType();
    }
}
