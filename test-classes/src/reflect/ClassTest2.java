/**
 * Status: Fail
 */
public class Test10 {
	public static void main(String[] args) {
		System.out.println(String.class.getName()); // java.lang.String
		System.out.println(byte.class.getName()); // byte
		System.out.println((new Object[3]).getClass().getName()); // [Ljava.lang.Object;
		System.out.println((new int[3][4][5][6][7][8][9]).getClass().getName()); // [[[[[[[I
	}
}