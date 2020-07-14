package reflect;

/**
 * Status: Pass
 */
public class ClassTest2 {
	public static void main(String[] args) {
		System.out.println(String.class.getName()); // java.lang.String
		System.out.println(byte.class.getName()); // byte
		System.out.println(Object[].class.getName()); // [Ljava.lang.Object;
		System.out.println(int[][][][][][][].class.getName()); // [[[[[[[I
	}
}