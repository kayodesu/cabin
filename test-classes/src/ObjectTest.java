
public class ObjectTest {
    public static int staticVar;
    public int instanceVar;

    @Utils.TestMethod(pass = false)
    public static void test() {
        int x = 32768; // ldc
        ObjectTest obj = new ObjectTest(); // new
        ObjectTest.staticVar = x; // putstatic
        x = ObjectTest.staticVar; // getstatic
        obj.instanceVar = x; // putfield
        x = obj.instanceVar; // getfield
        Object tmp = obj;
        if (tmp instanceof ObjectTest) { // instanceof
            obj = (ObjectTest) tmp; // checkcast
            System.out.println(obj.instanceVar == 32768 ? "Pass":"Fail");
            System.out.println(staticVar == 32768 ? "Pass":"Fail");
        } else {
            System.out.println("Fail");
        }
    }

    @Utils.TestMethod(pass = false)
    public static void testEquals() {
        Class<ObjectTest> c1 = ObjectTest.class;
        Class<Integer> c2 = Integer.class;
        System.out.println(c1.equals(c2));

        String s1 = "abc";
        String s2 = "123";
        System.out.println(s1.equals(s2));
    }

    private static class Super {
        int x;
        long y;
    }

    private static class Sub extends Super {
        float a;
        double b;
    }

    @Utils.TestMethod(pass = false)
    public static void testInheritance() {
        Sub sub = new Sub();
        sub.x = 1;
        sub.y = 2L;
        sub.a = 3.14f;
        sub.b = 2.71828;

        int x = sub.x;
        long y = sub.y;
        float a = sub.a;
        double b = sub.b;

        Super sup = sub;
        long j = sup.x + sup.y;

        System.out.println((x == 1 && y == 2 && a == 3.14f && b == 2.71828 && j == 3L) ? "Pass":"Fail");
    }

    public static void main(String[] args) {
        Utils.invokeAllTestMethods(ObjectTest.class);
    }
}
