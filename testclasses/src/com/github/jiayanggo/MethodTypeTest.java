package com.github.jiayanggo;

import java.lang.invoke.MethodType;

public class MethodTypeTest {
    
    public static void main(String[] args) throws Exception {
        //MethodType.fromMethodDescriptorString("()V", null);
        MethodType.methodType(Void.class);
        //System.out.println(MethodHandle.class);
    }
    
}
