package invoke;

import java.lang.invoke.MethodType;

/**
 * Status: Pass
 */
public class MethodTypeTest {
    
    public static void main(String[] args) throws Exception {
        MethodType mt = MethodType.fromMethodDescriptorString("(ZBSIJFD)[[Ljava/lang/String;", null);
        System.out.println(mt);

        mt = MethodType.methodType(Void.class);
        System.out.println(mt);

        mt = MethodType.methodType(void.class, int.class, double.class);
        System.out.println(mt);

        Class<?>[] ptypes = { int.class, float.class };
        mt = MethodType.methodType(String.class, ptypes);
        System.out.println(mt);
    }
    
}
