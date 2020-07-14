package string;

/**
 * Status: Pass
 */
public class InternTest {
    public static void main(String[] args) {
        String s = "abc";

        String a = new String("abc");
        String b = new String("abc");
        System.out.println((a != b) ? "Pass":"Fail");

        String interned = a.intern();
        System.out.println((interned == s)  ? "Pass":"Fail");
        System.out.println((interned == b.intern())  ? "Pass":"Fail");
    }
}
