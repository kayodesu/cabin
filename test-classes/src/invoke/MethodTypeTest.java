package invoke;

import sun.invoke.util.BytecodeDescriptor;

import java.lang.invoke.MethodType;
import java.util.Arrays;
import java.util.List;

/**
 * Status: Fail
 */
public class MethodTypeTest {

    public static void main(String[] args) throws Exception {
        MethodType mt = MethodType.fromMethodDescriptorString("(ZBSIJFD)[[Ljava/lang/String;", null);
        printMT(mt);

        mt = MethodType.fromMethodDescriptorString("(ZZZZZZZZZZZZZZZZZZZZZ)Z", null);
        printMT(mt);

        mt = MethodType.fromMethodDescriptorString("(Ljava/lang/String;Ljava/lang/String;)Z", null);
        printMT(mt);

//
//        mt = MethodType.methodType(Void.class);
//        System.out.println(mt); // ()Void
//
//        mt = MethodType.methodType(void.class, int.class, double.class);
//        System.out.println(mt); // (int,double)void
//
//        Class<?>[] ptypes = { int.class, float.class };
//        mt = MethodType.methodType(String.class, ptypes);
//        System.out.println(mt); // (int,float)String
    }

    private static void printMT(MethodType mt) {
        System.out.println("--------------------------------");
        System.out.println(Arrays.toString(mt.parameterArray()));
        System.out.println(mt);
        System.out.println(mt.toMethodDescriptorString());
    }
}
