package primary;

/**
 * Status: Fail
 */
public class LongTest {
    private long value;
    
    public LongTest(long value) {
        this.value = value;
    }
    
    public long get() {
        return value;
    }
    
    public static void main(String[] args) {
        LongTest tl = new LongTest(8682522807148012L);
        System.out.println(tl.get());
    }
}