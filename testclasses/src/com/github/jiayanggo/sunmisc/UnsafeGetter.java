package com.github.jiayanggo.sunmisc;

import sun.misc.Unsafe;

import java.lang.reflect.Field;

public class UnsafeGetter {
    
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
    
}
