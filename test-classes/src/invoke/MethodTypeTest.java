package invoke;

import java.lang.invoke.MethodType;
import java.lang.ref.Reference;
import java.lang.ref.ReferenceQueue;
import java.lang.ref.WeakReference;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

/**
 * Status: Fail
 */
public class MethodTypeTest {

    /**
     * Simple implementation of weak concurrent intern set.
     *
     * @param <T> interned type
     */
    private static class ConcurrentWeakInternSet<T> {

        private final ConcurrentMap<ConcurrentWeakInternSet.WeakEntry<T>, ConcurrentWeakInternSet.WeakEntry<T>> map;
        private final ReferenceQueue<T> stale;

        public ConcurrentWeakInternSet() {
            this.map = new ConcurrentHashMap<>();
            this.stale = new ReferenceQueue<>();
        }

        /**
         * Get the existing interned element.
         * This method returns null if no element is interned.
         *
         * @param elem element to look up
         * @return the interned element
         */
        public T get(T elem) {
            if (elem == null) throw new NullPointerException();
            expungeStaleElements();

            ConcurrentWeakInternSet.WeakEntry<T> value = map.get(new ConcurrentWeakInternSet.WeakEntry<>(elem));
            if (value != null) {
                T res = value.get();
                if (res != null) {
                    return res;
                }
            }
            return null;
        }

        /**
         * Interns the element.
         * Always returns non-null element, matching the one in the intern set.
         * Under the race against another add(), it can return <i>different</i>
         * element, if another thread beats us to interning it.
         *
         * @param elem element to add
         * @return element that was actually added
         */
        public T add(T elem) {
            if (elem == null) throw new NullPointerException();

            // Playing double race here, and so spinloop is required.
            // First race is with two concurrent updaters.
            // Second race is with GC purging weak ref under our feet.
            // Hopefully, we almost always end up with a single pass.
            T interned;
            ConcurrentWeakInternSet.WeakEntry<T> e = new ConcurrentWeakInternSet.WeakEntry<>(elem, stale);
            do {
                expungeStaleElements();
                ConcurrentWeakInternSet.WeakEntry<T> exist = map.putIfAbsent(e, e);
                interned = (exist == null) ? elem : exist.get();
            } while (interned == null);
            return interned;
        }

        private void expungeStaleElements() {
            Reference<? extends T> reference;
            while ((reference = stale.poll()) != null) {
                map.remove(reference);
            }
        }

        private static class WeakEntry<T> extends WeakReference<T> {

            public final int hashcode;

            public WeakEntry(T key, ReferenceQueue<T> queue) {
                super(key, queue);
                hashcode = key.hashCode();
            }

            public WeakEntry(T key) {
                super(key);
                hashcode = key.hashCode();
            }

            @Override
            public boolean equals(Object obj) {
                if (obj instanceof ConcurrentWeakInternSet.WeakEntry) {
                    Object that = ((ConcurrentWeakInternSet.WeakEntry) obj).get();
                    Object mine = get();
                    return (that == null || mine == null) ? (this == obj) : mine.equals(that);
                }
                return false;
            }

            @Override
            public int hashCode() {
                return hashcode;
            }

        }
    }

    static final ConcurrentWeakInternSet<MethodTypeTest> internTable = new ConcurrentWeakInternSet<>();

    private final Class<?>   rtype;
    private final Class<?>[] ptypes;

    private MethodTypeTest(Class<?>[] ptypes, Class<?> rtype) {
        this.rtype = rtype;
        this.ptypes = ptypes;
    }

    static MethodTypeTest makeImpl(Class<?> rtype, Class<?>[] ptypes, boolean trusted) {
        MethodTypeTest mt = internTable.get(new MethodTypeTest(ptypes, rtype));
        if (mt != null)
            return mt;
//        if (ptypes.length == 0) {
//            ptypes = NO_PTYPES; trusted = true;
//        }
        mt = new MethodTypeTest(ptypes, rtype);
//        // promote the object to the Real Thing, and reprobe
//        mt.form = MethodTypeForm.findForm(mt);
        return internTable.add(mt);
    }


    public static void main(String[] args) throws Exception {
        Class<?> rtype = Integer.class;
        Class<?>[] ptypes = { boolean.class, String.class };

//        MethodTypeTest mt = makeImpl(rtype, ptypes, true);
//        System.out.println(Arrays.toString(mt.ptypes));
//        System.out.println(mt.rtype);
//        System.out.println("----------------------------------------------");


//        Method makeImpl = class.getDeclaredMethod("makeImpl", Class.class, Class[].class, boolean.class);
//        makeImpl.setAccessible(true);
//        System.out.println(makeImpl);
//
//        Class<?> rtype = Integer.class;
//        Class<?>[] ptypes = { boolean.class, String.class };
//        MethodType mt = (MethodType) makeImpl.invoke(null, rtype, ptypes, true);
//        printMT(mt);


//        Class<?> rtype = Integer.class;
//        MethodType mt1 = methodType(rtype, Integer.class);
//        MethodType mt2 = methodType(rtype, Object.class);
//        printMT(mt1);
//        printMT(mt2);
//        System.out.println(Arrays.equals(new Object[]{Integer.class}, new Object[]{Object.class}));

        Class<?> c = MethodTypeTest.class.getClassLoader().loadClass("java.lang.invoke.MethodTypeForm");
        Method findForm = c.getDeclaredMethod("findForm", MethodType.class);
        findForm.setAccessible(true);

        Constructor<?> constructor = MethodType.class.getDeclaredConstructor(Class[].class, Class.class);
        constructor.setAccessible(true);
        MethodType mt1 = (MethodType) constructor.newInstance(ptypes, rtype);
//        MethodType mt1 = MethodType(ptypes, rtype);
        printMT(mt1);
        findForm.invoke(null, mt1);
        printMT(mt1);

//        MethodType mt = fromMethodDescriptorString("(ZBSIJFD)[[Ljava/lang/String;", null);
//        printMT(mt);
//
//        mt = fromMethodDescriptorString("(ZZZZZZZZZZZZZZZZZZZZZ)Z", null);
//        printMT(mt);
//
//        mt = fromMethodDescriptorString("(Ljava/lang/String;Ljava/lang/String;)Z", null);
//        printMT(mt);

//
//        mt = methodType(Void.class);
//        System.out.println(mt); // ()Void
//
//        mt = methodType(void.class, int.class, double.class);
//        System.out.println(mt); // (int,double)void
//
//        Class<?>[] ptypes = { int.class, float.class };
//        mt = methodType(String.class, ptypes);
//        System.out.println(mt); // (int,float)String
    }

    private static void printMT(MethodType mt) {
        System.out.println("--------------------------------");
        System.out.println(Arrays.toString(mt.parameterArray()));
        System.out.println(mt);
        System.out.println(mt.toMethodDescriptorString());
    }
}
