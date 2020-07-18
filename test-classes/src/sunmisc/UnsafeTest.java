package sunmisc;

import sun.misc.Unsafe;
import java.lang.reflect.Field;

/**
 * Status: Pass
 */
public class UnsafeTest {
    
    public static Unsafe getUnsafe() {
        //Unsafe unsafe = Unsafe.getUnsafe();
        try {
            Field f = Unsafe.class.getDeclaredField("theUnsafe");
            f.setAccessible(true);
            Unsafe unsafe = (Unsafe) f.get(null);
            return unsafe;
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public static void main(String[] args) {
        Unsafe unsafe = getUnsafe();
        System.out.println("page size: " + unsafe.pageSize());
    }
}
