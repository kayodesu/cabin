
public class IntegerCacheTest {
    public static void main(String []args) {
        Integer a = -128;
        Integer b = -128;
        System.out.println(a == b); // true
        
        Integer a1 = 127;
        Integer b1 = 127;
        System.out.println(a1 == b1); // true
        
        Integer c = -129;
        Integer d = -129;
        System.out.println(c == d); // false
        
        Integer c1 = 128;
        Integer d1 = 128;
        System.out.println(c1 == d1); // false
        
        System.out.println(new Integer(1) == new Integer(1)); // false
    }
}
