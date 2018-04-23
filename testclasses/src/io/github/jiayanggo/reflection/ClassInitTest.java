package io.github.jiayanggo.reflection;

public class ClassInitTest {
    
    static class A {
        public static int a = 100;
    }
    
    public static void main(String[] args) {
        ClassInitTest test = new ClassInitTest();
        test.getStatic();
    }

    public void getStatic() {
        try {
            Integer a = (Integer) A.class.getField("a").get(null);
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }
    
}
