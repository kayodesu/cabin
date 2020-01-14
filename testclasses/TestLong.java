
public class TestLong {
    private long value;
    
    public TestLong(long value) {
        this.value = value;
    }
    
    public long get() {
        return value;
    }
    
    public static void main(String[] args) {
        TestLong tl = new TestLong(8682522807148012L);
        System.out.println(tl.get());
    }
}