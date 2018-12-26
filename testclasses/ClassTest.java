
public class ClassTest {
    public static void main(String[] args) {
        Object obj = new Object();
        Class c1 = obj.getClass();
        Class c2 = c1.getClass();
        Class c3 = c2.getClass();
        System.out.println(c1);
        System.out.println(c2);
        System.out.println(c3);
        System.out.println(c2 == c1);
        System.out.println(c2 == c3);
    }
}
