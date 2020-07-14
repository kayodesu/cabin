package reflect;

import java.lang.reflect.Field;

/**
 * Status: Fail
 */
public class FieldTest {
    
    static int x = 123;
    int y;

    public FieldTest(int y) { this.y = y; }
    
    public static void main(String[] args) throws Exception {
        Field fx = FieldTest.class.getDeclaredField("x");
        fx.setAccessible(true);
        System.out.println((Integer) fx.get(null) == 123 ? "Pass":"Fail");

        Field fy = FieldTest.class.getDeclaredField("y");
        fy.setAccessible(true);
        System.out.println((Integer) fy.get(new FieldTest(456)) == 456 ? "Pass":"Fail");
    }
    
}
