
public class InvokeFuncTest1 {
    public static void main(String[] args) {
        for (int i = 0; i < 5; i++)
            new InvokeFuncTest1().test(i);
    }
    
    private void test(int i) {
        System.out.println(i);
    }
}
