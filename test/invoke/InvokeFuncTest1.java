
public class InvokeFuncTest1 {
    public static void main(String[] args) {
        for (int i = 0; i < 5; i++)
            System.out.println(new InvokeFuncTest1().test(i, i+1, i+2));
    }
    
    private int test(int i, int j, int k) {
        String s = i + "" + j + "" + k;
        System.out.println(s);
        return i + j + k;
    }
}
