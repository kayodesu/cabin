package reflect;

import java.util.Arrays;

/**
 * Status: Pass
 */
public class ClassTest4<T> {
    public static void main(String[] args) {
        printClassInfo(int.class);
        printClassInfo(int[].class);
        printClassInfo(int[][][].class);

        printClassInfo(ClassTest4.class);
    }

    private static void printClassInfo(Class<?> c) {
        System.out.println("----------");
        System.out.println(c.getName());
        System.out.println(c.getSimpleName());
        System.out.println(c.getModifiers());
        System.out.println(c.desiredAssertionStatus());
        System.out.println(c.isInterface());
        System.out.println(c.isArray());
        System.out.println(c.isPrimitive());
        System.out.println(c.getComponentType());
        System.out.println(Arrays.toString(c.getSigners()));
        System.out.println(Arrays.toString(c.getInterfaces()));
        System.out.println(c.getEnclosingMethod());
        System.out.println(Arrays.toString(c.getAnnotations()));
        System.out.println(c.getDeclaringClass());
        System.out.println(Arrays.toString(c.getDeclaredClasses()));
    }
}
