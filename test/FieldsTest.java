
public class FieldsTest {
    
    static class Sup {
        static int x;
        int a;
    }
    
    static class Sub extends Sup {
        static int y;
        int b;
    }

    public void staticFields() {
        int z = Sub.x + Sub.y;
        z += 100;
        Sub.y = z;
        Sub.x = z;
//        assertTrue(Sub.x == 100 && Sub.y == 100);
    }

    public void instanceFields() {
        Sub sub = new Sub();
        int c = sub.a + sub.b;
        c += 100;
        sub.a = c;
        sub.b = c;
//        assertTrue(sub.a == 100 && sub.b == 100);
    }
    
    public static void main(String[] args) {
        FieldsTest ft = new FieldsTest();
        ft.instanceFields();
        ft.staticFields();
    }
    
}
