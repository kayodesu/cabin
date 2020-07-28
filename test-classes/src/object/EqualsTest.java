package object;

public class EqualsTest {
    public static void main(String[] args) {
        Class<EqualsTest> c1 = EqualsTest.class;
        Class<Integer> c2 = Integer.class;
        System.out.println(c1.equals(c2));

        String s1 = "abc";
        String s2 = "123";
        System.out.println(s1.equals(s2));
    }
}
