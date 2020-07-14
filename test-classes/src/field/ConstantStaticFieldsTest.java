package field;

/**
 * A constant variable is a final variable of primitive type or type String
 * that is initialized with a constant expression (§15.28).
 *
 * Status: Fail
 */
public class ConstantStaticFieldsTest {
    
    public static final boolean z = true;
    public static final byte b = 125;
    public static final char c = 'c';
    public static final short s = 300;
    public static final int x = 100;
    public static final int y = x + 18;
    public static final long j = 1L;
    public static final float f = 3.14f;
    public static final double d = 2.71828;
    public static final String str1 = "hello";
    public static final String str2 = str1 + " world!";

    public void test() {
        System.out.println(true == ConstantStaticFieldsTest.z);
        System.out.println(true == (Boolean) getFieldValue("z"));
        System.out.println((byte)125 == (Byte) ConstantStaticFieldsTest.b);
        System.out.println((byte)125 == (Byte) getFieldValue("b"));
        System.out.println('c' == ConstantStaticFieldsTest.c);
        System.out.println('c' == (Character) getFieldValue("c"));
        System.out.println((short)300 == (Short) ConstantStaticFieldsTest.s);
        System.out.println((short)300 == (Short) getFieldValue("s"));
        System.out.println(100 == ConstantStaticFieldsTest.x);
        System.out.println(100 == (Integer) getFieldValue("x"));
        System.out.println(118 == ConstantStaticFieldsTest.y);
        System.out.println(118 == (Integer) getFieldValue("y"));
        System.out.println(1L == ConstantStaticFieldsTest.j);
        System.out.println(1L == (Long) getFieldValue("j"));
        System.out.println(3.14f == (Float) ConstantStaticFieldsTest.f);
        System.out.println(3.14f == (Float) getFieldValue("f"));
        System.out.println(2.71828 == (Double) ConstantStaticFieldsTest.d);
        System.out.println(2.71828 == (Double) getFieldValue("d"));
        System.out.println("hello".equals(ConstantStaticFieldsTest.str1));
        System.out.println("hello".equals(getFieldValue("str1")));
        System.out.println("hello world!".equals(ConstantStaticFieldsTest.str2));
        System.out.println("hello world!".equals(getFieldValue("str2")));
    }
    
    private static Object getFieldValue(String name) {
        try {
            return ConstantStaticFieldsTest.class.getField(name).get(null);
        } catch (ReflectiveOperationException e) {
            throw new RuntimeException(e);
        }
    }
    
    public static void main(String[] args) {
        new ConstantStaticFieldsTest().test();
    }
    
}
