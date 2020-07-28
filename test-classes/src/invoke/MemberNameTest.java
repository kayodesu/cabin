package invoke;

public class MemberNameTest {
    public static void main(String[] args) throws ClassNotFoundException {
        ClassLoader loader = MemberNameTest.class.getClassLoader();
        Class<?> c = loader.loadClass("java.lang.invoke.MemberName");
        System.out.println(c.getDeclaringClass());
        System.out.println(c.getName());
        System.out.println(c.getTypeName());
        System.out.println(c.getModifiers());
    }
}
