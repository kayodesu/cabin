package invoke;

public class MemberNameTest {
    public static void main(String[] args) throws ClassNotFoundException {
        ClassLoader loader = MemberNameTest.class.getClassLoader();
        Class<?> c = loader.load_class("java.lang.invoke.MemberName");
        System.out.println(c.getDeclaringClass()); // null
        System.out.println(c.getName()); // java.lang.invoke.MemberName
        System.out.println(c.getTypeName()); // java.lang.invoke.MemberName
        System.out.println(c.getModifiers()); // 16
    }
}
