package interface0;

import java.util.Arrays;

public class InterfaceMethodTest1 {

    interface Foo {
        default void test() { }
    }

    public static void main(String[] args) {
        System.out.println(Arrays.toString(Foo.class.getDeclaredMethods()));
    }
}
