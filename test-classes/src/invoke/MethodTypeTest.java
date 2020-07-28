package invoke;

import sun.invoke.util.BytecodeDescriptor;

import java.lang.invoke.MethodType;
import java.util.Arrays;
import java.util.List;

/**
 * Status: Fail
 */
public class MethodTypeTest {
    static final Class<?>[] NO_PTYPES = {};
    public static void main(String[] args) throws Exception {
        List<Class<?>> list = BytecodeDescriptor.parseMethod("([[Ljava/lang/String;)V", null);
        System.out.println(list); // [class [[Ljava.lang.String;, void]

        Class<?>[] xxx = list.toArray(NO_PTYPES);
        System.out.println(Arrays.toString(xxx));
        System.out.println(Arrays.toString(Arrays.copyOf(xxx, xxx.length)));

//        MethodType mt = MethodType.fromMethodDescriptorString("(ZBSIJFD)[[Ljava/lang/String;", null);
//        System.out.println(mt); // (boolean,byte,short,int,long,float,double)String[][]
//
        MethodType mt = MethodType.fromMethodDescriptorString("([[Ljava/lang/String;)V", null);
        System.out.println(mt); // (String[][])void
        String s = mt.toMethodDescriptorString();
        System.out.println(s); // ([[Ljava/lang/String;)V
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
    
}
