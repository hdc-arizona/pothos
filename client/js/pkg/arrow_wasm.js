
let wasm;

let cachedTextDecoder = new TextDecoder('utf-8', { ignoreBOM: true, fatal: true });

cachedTextDecoder.decode();

let cachegetUint8Memory0 = null;
function getUint8Memory0() {
    if (cachegetUint8Memory0 === null || cachegetUint8Memory0.buffer !== wasm.memory.buffer) {
        cachegetUint8Memory0 = new Uint8Array(wasm.memory.buffer);
    }
    return cachegetUint8Memory0;
}

function getStringFromWasm0(ptr, len) {
    return cachedTextDecoder.decode(getUint8Memory0().subarray(ptr, ptr + len));
}

const heap = new Array(32).fill(undefined);

heap.push(undefined, null, true, false);

let heap_next = heap.length;

function addHeapObject(obj) {
    if (heap_next === heap.length) heap.push(heap.length + 1);
    const idx = heap_next;
    heap_next = heap[idx];

    heap[idx] = obj;
    return idx;
}

function getObject(idx) { return heap[idx]; }

let WASM_VECTOR_LEN = 0;

let cachedTextEncoder = new TextEncoder('utf-8');

const encodeString = (typeof cachedTextEncoder.encodeInto === 'function'
    ? function (arg, view) {
    return cachedTextEncoder.encodeInto(arg, view);
}
    : function (arg, view) {
    const buf = cachedTextEncoder.encode(arg);
    view.set(buf);
    return {
        read: arg.length,
        written: buf.length
    };
});

function passStringToWasm0(arg, malloc, realloc) {

    if (realloc === undefined) {
        const buf = cachedTextEncoder.encode(arg);
        const ptr = malloc(buf.length);
        getUint8Memory0().subarray(ptr, ptr + buf.length).set(buf);
        WASM_VECTOR_LEN = buf.length;
        return ptr;
    }

    let len = arg.length;
    let ptr = malloc(len);

    const mem = getUint8Memory0();

    let offset = 0;

    for (; offset < len; offset++) {
        const code = arg.charCodeAt(offset);
        if (code > 0x7F) break;
        mem[ptr + offset] = code;
    }

    if (offset !== len) {
        if (offset !== 0) {
            arg = arg.slice(offset);
        }
        ptr = realloc(ptr, len, len = offset + arg.length * 3);
        const view = getUint8Memory0().subarray(ptr + offset, ptr + len);
        const ret = encodeString(arg, view);

        offset += ret.written;
    }

    WASM_VECTOR_LEN = offset;
    return ptr;
}

let cachegetInt32Memory0 = null;
function getInt32Memory0() {
    if (cachegetInt32Memory0 === null || cachegetInt32Memory0.buffer !== wasm.memory.buffer) {
        cachegetInt32Memory0 = new Int32Array(wasm.memory.buffer);
    }
    return cachegetInt32Memory0;
}

function dropObject(idx) {
    if (idx < 36) return;
    heap[idx] = heap_next;
    heap_next = idx;
}

function takeObject(idx) {
    const ret = getObject(idx);
    dropObject(idx);
    return ret;
}

let stack_pointer = 32;

function addBorrowedObject(obj) {
    if (stack_pointer == 1) throw new Error('out of js stack');
    heap[--stack_pointer] = obj;
    return stack_pointer;
}

const u32CvtShim = new Uint32Array(2);

const int64CvtShim = new BigInt64Array(u32CvtShim.buffer);

function passArray8ToWasm0(arg, malloc) {
    const ptr = malloc(arg.length * 1);
    getUint8Memory0().set(arg, ptr / 1);
    WASM_VECTOR_LEN = arg.length;
    return ptr;
}

function _assertClass(instance, klass) {
    if (!(instance instanceof klass)) {
        throw new Error(`expected instance of ${klass.name}`);
    }
    return instance.ptr;
}

let cachegetUint16Memory0 = null;
function getUint16Memory0() {
    if (cachegetUint16Memory0 === null || cachegetUint16Memory0.buffer !== wasm.memory.buffer) {
        cachegetUint16Memory0 = new Uint16Array(wasm.memory.buffer);
    }
    return cachegetUint16Memory0;
}

function passArray16ToWasm0(arg, malloc) {
    const ptr = malloc(arg.length * 2);
    getUint16Memory0().set(arg, ptr / 2);
    WASM_VECTOR_LEN = arg.length;
    return ptr;
}

let cachegetUint32Memory0 = null;
function getUint32Memory0() {
    if (cachegetUint32Memory0 === null || cachegetUint32Memory0.buffer !== wasm.memory.buffer) {
        cachegetUint32Memory0 = new Uint32Array(wasm.memory.buffer);
    }
    return cachegetUint32Memory0;
}

function passArray32ToWasm0(arg, malloc) {
    const ptr = malloc(arg.length * 4);
    getUint32Memory0().set(arg, ptr / 4);
    WASM_VECTOR_LEN = arg.length;
    return ptr;
}

let cachegetUint64Memory0 = null;
function getUint64Memory0() {
    if (cachegetUint64Memory0 === null || cachegetUint64Memory0.buffer !== wasm.memory.buffer) {
        cachegetUint64Memory0 = new BigUint64Array(wasm.memory.buffer);
    }
    return cachegetUint64Memory0;
}

function passArray64ToWasm0(arg, malloc) {
    const ptr = malloc(arg.length * 8);
    getUint64Memory0().set(arg, ptr / 8);
    WASM_VECTOR_LEN = arg.length;
    return ptr;
}

let cachegetInt64Memory0 = null;
function getInt64Memory0() {
    if (cachegetInt64Memory0 === null || cachegetInt64Memory0.buffer !== wasm.memory.buffer) {
        cachegetInt64Memory0 = new BigInt64Array(wasm.memory.buffer);
    }
    return cachegetInt64Memory0;
}

function getArrayI64FromWasm0(ptr, len) {
    return getInt64Memory0().subarray(ptr / 8, ptr / 8 + len);
}

const uint64CvtShim = new BigUint64Array(u32CvtShim.buffer);

function getArrayU64FromWasm0(ptr, len) {
    return getUint64Memory0().subarray(ptr / 8, ptr / 8 + len);
}

let cachegetFloat32Memory0 = null;
function getFloat32Memory0() {
    if (cachegetFloat32Memory0 === null || cachegetFloat32Memory0.buffer !== wasm.memory.buffer) {
        cachegetFloat32Memory0 = new Float32Array(wasm.memory.buffer);
    }
    return cachegetFloat32Memory0;
}

function passArrayF32ToWasm0(arg, malloc) {
    const ptr = malloc(arg.length * 4);
    getFloat32Memory0().set(arg, ptr / 4);
    WASM_VECTOR_LEN = arg.length;
    return ptr;
}

let cachegetFloat64Memory0 = null;
function getFloat64Memory0() {
    if (cachegetFloat64Memory0 === null || cachegetFloat64Memory0.buffer !== wasm.memory.buffer) {
        cachegetFloat64Memory0 = new Float64Array(wasm.memory.buffer);
    }
    return cachegetFloat64Memory0;
}

function passArrayF64ToWasm0(arg, malloc) {
    const ptr = malloc(arg.length * 8);
    getFloat64Memory0().set(arg, ptr / 8);
    WASM_VECTOR_LEN = arg.length;
    return ptr;
}

function getArrayU8FromWasm0(ptr, len) {
    return getUint8Memory0().subarray(ptr / 1, ptr / 1 + len);
}
/**
*/
export class ArrowVectorFFI {

    static __wrap(ptr) {
        const obj = Object.create(ArrowVectorFFI.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_arrowvectorffi_free(ptr);
    }
    /**
    * @returns {number}
    */
    get array() {
        var ret = wasm.__wbg_get_arrowvectorffi_array(this.ptr);
        return ret;
    }
    /**
    * @param {number} arg0
    */
    set array(arg0) {
        wasm.__wbg_set_arrowvectorffi_array(this.ptr, arg0);
    }
    /**
    * @returns {number}
    */
    get schema() {
        var ret = wasm.__wbg_get_arrowvectorffi_schema(this.ptr);
        return ret;
    }
    /**
    * @param {number} arg0
    */
    set schema(arg0) {
        wasm.__wbg_set_arrowvectorffi_schema(this.ptr, arg0);
    }
}
/**
*/
export class BooleanVector {

    static __wrap(ptr) {
        const obj = Object.create(BooleanVector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_booleanvector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.booleanvector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.booleanvector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.booleanvector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.booleanvector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.booleanvector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.booleanvector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.booleanvector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {boolean}
    */
    get(index) {
        var ret = wasm.booleanvector_get(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {BooleanVector}
    */
    slice(offset, length) {
        var ret = wasm.booleanvector_slice(this.ptr, offset, length);
        return BooleanVector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {BooleanVector}
    */
    limit(num_elements) {
        var ret = wasm.booleanvector_limit(this.ptr, num_elements);
        return BooleanVector.__wrap(ret);
    }
    /**
    * @param {Uint8Array} data
    * @param {number} length
    * @returns {BooleanVector}
    */
    static from(data, length) {
        var ptr0 = passArray8ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.booleanvector_from(ptr0, len0, length);
        return BooleanVector.__wrap(ret);
    }
    /**
    * Returns a `Buffer` holding all the values of this array.
    *
    * Note this doesn't take the offset of this array into account.
    * @returns {Uint8Array}
    */
    toArray() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.booleanvector_toArray(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var v0 = getArrayU8FromWasm0(r0, r1).slice();
            wasm.__wbindgen_free(r0, r1 * 1);
            return v0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * Creates a JS typed array which is a view into wasm's linear memory at the slice specified.
    * This function returns a new typed array which is a view into wasm's memory. This view does not copy the underlying data.
    * @returns {Uint8Array}
    */
    view() {
        var ret = wasm.booleanvector_view(this.ptr);
        return takeObject(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.booleanvector_toJSON(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class DataType {

    static __wrap(ptr) {
        const obj = Object.create(DataType.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_datatype_free(ptr);
    }
    /**
    * Generate a JSON representation.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.datatype_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.datatype_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Parse a `Field` definition from a JSON representation.
    * @param {any} json
    * @returns {DataType}
    */
    static from(json) {
        try {
            var ret = wasm.datatype_from(addBorrowedObject(json));
            return DataType.__wrap(ret);
        } finally {
            heap[stack_pointer++] = undefined;
        }
    }
}
/**
*/
export class Field {

    static __wrap(ptr) {
        const obj = Object.create(Field.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_field_free(ptr);
    }
    /**
    * Generate a JSON representation.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.field_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.field_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * @returns {string}
    */
    get name() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.field_name(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * @returns {boolean}
    */
    get isNullable() {
        var ret = wasm.field_isNullable(this.ptr);
        return ret !== 0;
    }
    /**
    * @returns {BigInt | undefined}
    */
    get dictId() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.field_dictId(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var r2 = getInt32Memory0()[retptr / 4 + 2];
            u32CvtShim[0] = r1;
            u32CvtShim[1] = r2;
            const n0 = r0 === 0 ? undefined : int64CvtShim[0];
            return n0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {boolean | undefined}
    */
    get dictIsOrdered() {
        var ret = wasm.field_dictIsOrdered(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret !== 0;
    }
    /**
    * @returns {DataType}
    */
    get dataType() {
        var ret = wasm.field_dataType(this.ptr);
        return DataType.__wrap(ret);
    }
    /**
    * Parse a `Field` definition from a JSON representation.
    * @param {any} json
    * @returns {Field}
    */
    static from(json) {
        try {
            var ret = wasm.field_from(addBorrowedObject(json));
            return Field.__wrap(ret);
        } finally {
            heap[stack_pointer++] = undefined;
        }
    }
}
/**
*/
export class Float32Vector {

    static __wrap(ptr) {
        const obj = Object.create(Float32Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_float32vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.float32vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.float32vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {number}
    */
    get(index) {
        var ret = wasm.float32vector_get(this.ptr, index);
        return ret;
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Float32Vector}
    */
    slice(offset, length) {
        var ret = wasm.float32vector_slice(this.ptr, offset, length);
        return Float32Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Float32Vector}
    */
    limit(num_elements) {
        var ret = wasm.float32vector_limit(this.ptr, num_elements);
        return Float32Vector.__wrap(ret);
    }
    /**
    * @param {Float32Array} data
    * @returns {Float32Vector}
    */
    static from(data) {
        var ptr0 = passArrayF32ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.float32vector_from(ptr0, len0);
        return Float32Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.float32vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {number | undefined}
    */
    sum() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.float32vector_sum(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getFloat32Memory0()[retptr / 4 + 1];
            return r0 === 0 ? undefined : r1;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {number | undefined}
    */
    min() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.float32vector_min(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getFloat32Memory0()[retptr / 4 + 1];
            return r0 === 0 ? undefined : r1;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {number | undefined}
    */
    max() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.float32vector_max(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getFloat32Memory0()[retptr / 4 + 1];
            return r0 === 0 ? undefined : r1;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {Float32Array}
    */
    toArray() {
        var ret = wasm.float32vector_toArray(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {Float32Array}
    */
    view() {
        var ret = wasm.float32vector_view(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class Float64Vector {

    static __wrap(ptr) {
        const obj = Object.create(Float64Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_float64vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.float64vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.float32vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {number}
    */
    get(index) {
        var ret = wasm.float64vector_get(this.ptr, index);
        return ret;
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Float64Vector}
    */
    slice(offset, length) {
        var ret = wasm.float64vector_slice(this.ptr, offset, length);
        return Float64Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Float64Vector}
    */
    limit(num_elements) {
        var ret = wasm.float64vector_limit(this.ptr, num_elements);
        return Float64Vector.__wrap(ret);
    }
    /**
    * @param {Float64Array} data
    * @returns {Float64Vector}
    */
    static from(data) {
        var ptr0 = passArrayF64ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.float64vector_from(ptr0, len0);
        return Float64Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.float64vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {number | undefined}
    */
    sum() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.float64vector_sum(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getFloat64Memory0()[retptr / 8 + 1];
            return r0 === 0 ? undefined : r1;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {number | undefined}
    */
    min() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.float64vector_min(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getFloat64Memory0()[retptr / 8 + 1];
            return r0 === 0 ? undefined : r1;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {number | undefined}
    */
    max() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.float64vector_max(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getFloat64Memory0()[retptr / 8 + 1];
            return r0 === 0 ? undefined : r1;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {Float64Array}
    */
    toArray() {
        var ret = wasm.float64vector_toArray(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {Float64Array}
    */
    view() {
        var ret = wasm.float64vector_view(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class Int16Vector {

    static __wrap(ptr) {
        const obj = Object.create(Int16Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_int16vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int16vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.float32vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {number}
    */
    get(index) {
        var ret = wasm.int16vector_get(this.ptr, index);
        return ret;
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Int16Vector}
    */
    slice(offset, length) {
        var ret = wasm.int16vector_slice(this.ptr, offset, length);
        return Int16Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Int16Vector}
    */
    limit(num_elements) {
        var ret = wasm.int16vector_limit(this.ptr, num_elements);
        return Int16Vector.__wrap(ret);
    }
    /**
    * @param {Int16Array} data
    * @returns {Int16Vector}
    */
    static from(data) {
        var ptr0 = passArray16ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.int16vector_from(ptr0, len0);
        return Int16Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.int16vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {number | undefined}
    */
    sum() {
        var ret = wasm.int16vector_sum(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * @returns {number | undefined}
    */
    min() {
        var ret = wasm.int16vector_min(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * @returns {number | undefined}
    */
    max() {
        var ret = wasm.int16vector_max(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {Int16Array}
    */
    toArray() {
        var ret = wasm.int16vector_toArray(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {Int16Array}
    */
    view() {
        var ret = wasm.int16vector_view(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class Int32Vector {

    static __wrap(ptr) {
        const obj = Object.create(Int32Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_int32vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int32vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.float32vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {number}
    */
    get(index) {
        var ret = wasm.int32vector_get(this.ptr, index);
        return ret;
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Int32Vector}
    */
    slice(offset, length) {
        var ret = wasm.float32vector_slice(this.ptr, offset, length);
        return Int32Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Int32Vector}
    */
    limit(num_elements) {
        var ret = wasm.float32vector_limit(this.ptr, num_elements);
        return Int32Vector.__wrap(ret);
    }
    /**
    * @param {Int32Array} data
    * @returns {Int32Vector}
    */
    static from(data) {
        var ptr0 = passArray32ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.int32vector_from(ptr0, len0);
        return Int32Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.int32vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {number | undefined}
    */
    sum() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int32vector_sum(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return r0 === 0 ? undefined : r1;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {number | undefined}
    */
    min() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int32vector_min(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return r0 === 0 ? undefined : r1;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {number | undefined}
    */
    max() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int32vector_max(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return r0 === 0 ? undefined : r1;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {Int32Array}
    */
    toArray() {
        var ret = wasm.int32vector_toArray(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {Int32Array}
    */
    view() {
        var ret = wasm.int32vector_view(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class Int64Vector {

    static __wrap(ptr) {
        const obj = Object.create(Int64Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_int64vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int64vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.float32vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {BigInt}
    */
    get(index) {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int64vector_get(retptr, this.ptr, index);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            u32CvtShim[0] = r0;
            u32CvtShim[1] = r1;
            const n0 = int64CvtShim[0];
            return n0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Int64Vector}
    */
    slice(offset, length) {
        var ret = wasm.float64vector_slice(this.ptr, offset, length);
        return Int64Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Int64Vector}
    */
    limit(num_elements) {
        var ret = wasm.float64vector_limit(this.ptr, num_elements);
        return Int64Vector.__wrap(ret);
    }
    /**
    * @param {BigInt64Array} data
    * @returns {Int64Vector}
    */
    static from(data) {
        var ptr0 = passArray64ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.int64vector_from(ptr0, len0);
        return Int64Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.int64vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {BigInt | undefined}
    */
    sum() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int64vector_sum(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var r2 = getInt32Memory0()[retptr / 4 + 2];
            u32CvtShim[0] = r1;
            u32CvtShim[1] = r2;
            const n0 = r0 === 0 ? undefined : int64CvtShim[0];
            return n0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {BigInt | undefined}
    */
    min() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int64vector_min(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var r2 = getInt32Memory0()[retptr / 4 + 2];
            u32CvtShim[0] = r1;
            u32CvtShim[1] = r2;
            const n0 = r0 === 0 ? undefined : int64CvtShim[0];
            return n0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {BigInt | undefined}
    */
    max() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int64vector_max(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var r2 = getInt32Memory0()[retptr / 4 + 2];
            u32CvtShim[0] = r1;
            u32CvtShim[1] = r2;
            const n0 = r0 === 0 ? undefined : int64CvtShim[0];
            return n0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {BigInt64Array}
    */
    toArray() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int64vector_toArray(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var v0 = getArrayI64FromWasm0(r0, r1).slice();
            wasm.__wbindgen_free(r0, r1 * 8);
            return v0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
}
/**
*/
export class Int8Vector {

    static __wrap(ptr) {
        const obj = Object.create(Int8Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_int8vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int8vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.int8vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {number}
    */
    get(index) {
        var ret = wasm.int8vector_get(this.ptr, index);
        return ret;
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Int8Vector}
    */
    slice(offset, length) {
        var ret = wasm.int8vector_slice(this.ptr, offset, length);
        return Int8Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Int8Vector}
    */
    limit(num_elements) {
        var ret = wasm.int8vector_limit(this.ptr, num_elements);
        return Int8Vector.__wrap(ret);
    }
    /**
    * @param {Int8Array} data
    * @returns {Int8Vector}
    */
    static from(data) {
        var ptr0 = passArray8ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.int8vector_from(ptr0, len0);
        return Int8Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.int8vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {number | undefined}
    */
    sum() {
        var ret = wasm.int8vector_sum(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * @returns {number | undefined}
    */
    min() {
        var ret = wasm.int8vector_min(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * @returns {number | undefined}
    */
    max() {
        var ret = wasm.int8vector_max(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {Int8Array}
    */
    toArray() {
        var ret = wasm.int8vector_toArray(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {Int8Array}
    */
    view() {
        var ret = wasm.int8vector_view(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class RecordBatch {

    static __wrap(ptr) {
        const obj = Object.create(RecordBatch.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_recordbatch_free(ptr);
    }
    /**
    * @returns {number}
    */
    get numRows() {
        var ret = wasm.recordbatch_numRows(this.ptr);
        return ret >>> 0;
    }
    /**
    * @returns {number}
    */
    get numColumns() {
        var ret = wasm.recordbatch_numColumns(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns the schema of the record batches.
    * @returns {Schema}
    */
    get schema() {
        var ret = wasm.recordbatch_schema(this.ptr);
        return Schema.__wrap(ret);
    }
    /**
    * Get a column's vector by index.
    * @param {number} index
    * @returns {Vector}
    */
    column(index) {
        var ret = wasm.recordbatch_column(this.ptr, index);
        return Vector.__wrap(ret);
    }
    /**
    * Get all columns in the record batch.
    * @returns {Array<any>}
    */
    get columns() {
        var ret = wasm.recordbatch_columns(this.ptr);
        return takeObject(ret);
    }
    /**
    * Get a column's vector by name.
    * @param {string} name
    * @returns {Vector}
    */
    columnWithName(name) {
        var ptr0 = passStringToWasm0(name, wasm.__wbindgen_malloc, wasm.__wbindgen_realloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.recordbatch_columnWithName(this.ptr, ptr0, len0);
        return Vector.__wrap(ret);
    }
}
/**
*/
export class Schema {

    static __wrap(ptr) {
        const obj = Object.create(Schema.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_schema_free(ptr);
    }
    /**
    * Generate a JSON representation.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.schema_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.schema_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * @param {number} i
    * @returns {Field}
    */
    field(i) {
        var ret = wasm.schema_field(this.ptr, i);
        return Field.__wrap(ret);
    }
    /**
    * @returns {any}
    */
    get fields() {
        var ret = wasm.schema_fields(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {number}
    */
    get numFields() {
        var ret = wasm.schema_numFields(this.ptr);
        return ret >>> 0;
    }
    /**
    * Look up a column by name and return a immutable reference to the column along with its index.
    * @param {string} name
    * @returns {any}
    */
    columnWithName(name) {
        var ptr0 = passStringToWasm0(name, wasm.__wbindgen_malloc, wasm.__wbindgen_realloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.schema_columnWithName(this.ptr, ptr0, len0);
        return takeObject(ret);
    }
    /**
    * Find the index of the column with the given name.
    * @param {string} name
    * @returns {number}
    */
    indexOf(name) {
        var ptr0 = passStringToWasm0(name, wasm.__wbindgen_malloc, wasm.__wbindgen_realloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.schema_indexOf(this.ptr, ptr0, len0);
        return ret >>> 0;
    }
    /**
    * @param {string} name
    * @returns {Field}
    */
    fieldWithName(name) {
        var ptr0 = passStringToWasm0(name, wasm.__wbindgen_malloc, wasm.__wbindgen_realloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.schema_fieldWithName(this.ptr, ptr0, len0);
        return Field.__wrap(ret);
    }
    /**
    * Returns custom metadata key-value pairs.
    * @returns {any}
    */
    get metadata() {
        var ret = wasm.schema_metadata(this.ptr);
        return takeObject(ret);
    }
    /**
    * Parse a `Schema` definition from a JSON representation.
    * @param {any} json
    * @returns {Schema}
    */
    static from(json) {
        try {
            var ret = wasm.schema_from(addBorrowedObject(json));
            return Schema.__wrap(ret);
        } finally {
            heap[stack_pointer++] = undefined;
        }
    }
}
/**
* String vector
*/
export class StringVector {

    static __wrap(ptr) {
        const obj = Object.create(StringVector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_stringvector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.stringvector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.stringvector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.stringvector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.stringvector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.stringvector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.stringvector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.stringvector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {string}
    */
    get(index) {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.stringvector_get(retptr, this.ptr, index);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {StringVector}
    */
    slice(offset, length) {
        var ret = wasm.stringvector_slice(this.ptr, offset, length);
        return StringVector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {StringVector}
    */
    limit(num_elements) {
        var ret = wasm.stringvector_limit(this.ptr, num_elements);
        return StringVector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.stringvector_toJSON(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class Table {

    static __wrap(ptr) {
        const obj = Object.create(Table.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_table_free(ptr);
    }
    /**
    * Returns the schema of the record batches.
    * @returns {Schema}
    */
    get schema() {
        var ret = wasm.table_schema(this.ptr);
        return Schema.__wrap(ret);
    }
    /**
    * @param {number} index
    * @returns {RecordBatch | undefined}
    */
    recordBatch(index) {
        var ret = wasm.table_recordBatch(this.ptr, index);
        return ret === 0 ? undefined : RecordBatch.__wrap(ret);
    }
    /**
    * Return the number of batches in the file
    * @returns {number}
    */
    get numBatches() {
        var ret = wasm.table_numBatches(this.ptr);
        return ret >>> 0;
    }
    /**
    * Create a table from IPC bytes. Use `fromWasmUint8Array` to avoid memory copies.
    * @param {Uint8Array} contents
    * @returns {Table}
    */
    static from(contents) {
        var ptr0 = passArray8ToWasm0(contents, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.table_from(ptr0, len0);
        return Table.__wrap(ret);
    }
    /**
    * Create a table from a pre-initialized buffer. The memory is passed without a copy.
    * @param {WasmUint8Array} data
    * @returns {Table}
    */
    static fromWasmUint8Array(data) {
        _assertClass(data, WasmUint8Array);
        var ret = wasm.table_fromWasmUint8Array(data.ptr);
        return Table.__wrap(ret);
    }
    /**
    * @returns {Uint8Array}
    */
    serialize() {
        var ret = wasm.table_serialize(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class Uint16Vector {

    static __wrap(ptr) {
        const obj = Object.create(Uint16Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_uint16vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.uint16vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.float32vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {number}
    */
    get(index) {
        var ret = wasm.uint16vector_get(this.ptr, index);
        return ret;
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Uint16Vector}
    */
    slice(offset, length) {
        var ret = wasm.int16vector_slice(this.ptr, offset, length);
        return Uint16Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Uint16Vector}
    */
    limit(num_elements) {
        var ret = wasm.int16vector_limit(this.ptr, num_elements);
        return Uint16Vector.__wrap(ret);
    }
    /**
    * @param {Uint16Array} data
    * @returns {Uint16Vector}
    */
    static from(data) {
        var ptr0 = passArray16ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.uint16vector_from(ptr0, len0);
        return Uint16Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.uint16vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {number | undefined}
    */
    sum() {
        var ret = wasm.uint16vector_sum(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * @returns {number | undefined}
    */
    min() {
        var ret = wasm.uint16vector_min(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * @returns {number | undefined}
    */
    max() {
        var ret = wasm.uint16vector_max(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {Uint16Array}
    */
    toArray() {
        var ret = wasm.uint16vector_toArray(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {Uint16Array}
    */
    view() {
        var ret = wasm.uint16vector_view(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class Uint32Vector {

    static __wrap(ptr) {
        const obj = Object.create(Uint32Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_uint32vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.uint32vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.float32vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {number}
    */
    get(index) {
        var ret = wasm.int32vector_get(this.ptr, index);
        return ret >>> 0;
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Uint32Vector}
    */
    slice(offset, length) {
        var ret = wasm.float32vector_slice(this.ptr, offset, length);
        return Uint32Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Uint32Vector}
    */
    limit(num_elements) {
        var ret = wasm.float32vector_limit(this.ptr, num_elements);
        return Uint32Vector.__wrap(ret);
    }
    /**
    * @param {Uint32Array} data
    * @returns {Uint32Vector}
    */
    static from(data) {
        var ptr0 = passArray32ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.uint32vector_from(ptr0, len0);
        return Uint32Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.uint32vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {number | undefined}
    */
    sum() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int32vector_sum(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return r0 === 0 ? undefined : r1 >>> 0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {number | undefined}
    */
    min() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.uint32vector_min(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return r0 === 0 ? undefined : r1 >>> 0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {number | undefined}
    */
    max() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.uint32vector_max(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return r0 === 0 ? undefined : r1 >>> 0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {Uint32Array}
    */
    toArray() {
        var ret = wasm.uint32vector_toArray(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {Uint32Array}
    */
    view() {
        var ret = wasm.uint32vector_view(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class Uint64Vector {

    static __wrap(ptr) {
        const obj = Object.create(Uint64Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_uint64vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.uint64vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.float32vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {BigInt}
    */
    get(index) {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int64vector_get(retptr, this.ptr, index);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            u32CvtShim[0] = r0;
            u32CvtShim[1] = r1;
            const n0 = uint64CvtShim[0];
            return n0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Uint64Vector}
    */
    slice(offset, length) {
        var ret = wasm.float64vector_slice(this.ptr, offset, length);
        return Uint64Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Uint64Vector}
    */
    limit(num_elements) {
        var ret = wasm.float64vector_limit(this.ptr, num_elements);
        return Uint64Vector.__wrap(ret);
    }
    /**
    * @param {BigUint64Array} data
    * @returns {Uint64Vector}
    */
    static from(data) {
        var ptr0 = passArray64ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.uint64vector_from(ptr0, len0);
        return Uint64Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.uint64vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {BigInt | undefined}
    */
    sum() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int64vector_sum(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var r2 = getInt32Memory0()[retptr / 4 + 2];
            u32CvtShim[0] = r1;
            u32CvtShim[1] = r2;
            const n0 = r0 === 0 ? undefined : uint64CvtShim[0];
            return n0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {BigInt | undefined}
    */
    min() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.uint64vector_min(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var r2 = getInt32Memory0()[retptr / 4 + 2];
            u32CvtShim[0] = r1;
            u32CvtShim[1] = r2;
            const n0 = r0 === 0 ? undefined : uint64CvtShim[0];
            return n0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * @returns {BigInt | undefined}
    */
    max() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.uint64vector_max(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var r2 = getInt32Memory0()[retptr / 4 + 2];
            u32CvtShim[0] = r1;
            u32CvtShim[1] = r2;
            const n0 = r0 === 0 ? undefined : uint64CvtShim[0];
            return n0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {BigUint64Array}
    */
    toArray() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.int64vector_toArray(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            var v0 = getArrayU64FromWasm0(r0, r1).slice();
            wasm.__wbindgen_free(r0, r1 * 8);
            return v0;
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
        }
    }
}
/**
*/
export class Uint8Vector {

    static __wrap(ptr) {
        const obj = Object.create(Uint8Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_uint8vector_free(ptr);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.uint8vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.float32vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.float32vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.float32vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.float32vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.float32vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.int8vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    * Returns the primitive value at `index`.
    * @param {number} index
    * @returns {number}
    */
    get(index) {
        var ret = wasm.uint8vector_get(this.ptr, index);
        return ret;
    }
    /**
    * Returns a zero-copy slice of this array with the indicated offset and length.
    * @param {number} offset
    * @param {number} length
    * @returns {Uint8Vector}
    */
    slice(offset, length) {
        var ret = wasm.int8vector_slice(this.ptr, offset, length);
        return Uint8Vector.__wrap(ret);
    }
    /**
    * Returns the array, taking only the number of elements specified.
    * @param {number} num_elements
    * @returns {Uint8Vector}
    */
    limit(num_elements) {
        var ret = wasm.int8vector_limit(this.ptr, num_elements);
        return Uint8Vector.__wrap(ret);
    }
    /**
    * @param {Uint8Array} data
    * @returns {Uint8Vector}
    */
    static from(data) {
        var ptr0 = passArray8ToWasm0(data, wasm.__wbindgen_malloc);
        var len0 = WASM_VECTOR_LEN;
        var ret = wasm.uint8vector_from(ptr0, len0);
        return Uint8Vector.__wrap(ret);
    }
    /**
    * Returns the contents of the vector as a JSON array.
    * @returns {any}
    */
    toJSON() {
        var ret = wasm.uint8vector_toJSON(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {number | undefined}
    */
    sum() {
        var ret = wasm.uint8vector_sum(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * @returns {number | undefined}
    */
    min() {
        var ret = wasm.uint8vector_min(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * @returns {number | undefined}
    */
    max() {
        var ret = wasm.uint8vector_max(this.ptr);
        return ret === 0xFFFFFF ? undefined : ret;
    }
    /**
    * Returns the contents of the vector as a typed array.
    * @returns {Uint8Array}
    */
    toArray() {
        var ret = wasm.uint8vector_toArray(this.ptr);
        return takeObject(ret);
    }
    /**
    * @returns {Uint8Array}
    */
    view() {
        var ret = wasm.uint8vector_view(this.ptr);
        return takeObject(ret);
    }
}
/**
*/
export class Vector {

    static __wrap(ptr) {
        const obj = Object.create(Vector.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_vector_free(ptr);
    }
    /**
    * Make a vector from binary in the C Data Interface (FFI).
    * @param {number} array
    * @param {number} schema
    * @returns {Vector}
    */
    static fromRaw(array, schema) {
        var ret = wasm.vector_fromRaw(array, schema);
        return Vector.__wrap(ret);
    }
    /**
    * Generate a String representation.
    * @returns {string}
    */
    toString() {
        try {
            const retptr = wasm.__wbindgen_add_to_stack_pointer(-16);
            wasm.vector_toString(retptr, this.ptr);
            var r0 = getInt32Memory0()[retptr / 4 + 0];
            var r1 = getInt32Memory0()[retptr / 4 + 1];
            return getStringFromWasm0(r0, r1);
        } finally {
            wasm.__wbindgen_add_to_stack_pointer(16);
            wasm.__wbindgen_free(r0, r1);
        }
    }
    /**
    * Get the length of the vector.
    * @returns {number}
    */
    get length() {
        var ret = wasm.vector_length(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns whether the element at `index` is not null.
    * @param {number} index
    * @returns {boolean}
    */
    isValid(index) {
        var ret = wasm.vector_isValid(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether the element at `index` is null.
    * @param {number} index
    * @returns {boolean}
    */
    isNull(index) {
        var ret = wasm.vector_isNull(this.ptr, index);
        return ret !== 0;
    }
    /**
    * Returns whether this vector is empty.
    * @returns {boolean}
    */
    get isEmpty() {
        var ret = wasm.vector_isEmpty(this.ptr);
        return ret !== 0;
    }
    /**
    * Returns the total number of null values in this vector.
    * @returns {number}
    */
    get nullCount() {
        var ret = wasm.vector_nullCount(this.ptr);
        return ret >>> 0;
    }
    /**
    * Returns two pointers that represent this vector in the C Data Interface (FFI).
    * @returns {ArrowVectorFFI}
    */
    toRaw() {
        var ret = wasm.vector_toRaw(this.ptr);
        return ArrowVectorFFI.__wrap(ret);
    }
    /**
    *Cast Vector as a `Int8Vector`.
    * @returns {Int8Vector}
    */
    asInt8Vector() {
        var ret = wasm.vector_asInt8Vector(this.ptr);
        return Int8Vector.__wrap(ret);
    }
    /**
    *Cast Vector as a `Int16Vector`.
    * @returns {Int16Vector}
    */
    asInt16Vector() {
        var ret = wasm.vector_asInt16Vector(this.ptr);
        return Int16Vector.__wrap(ret);
    }
    /**
    *Cast Vector as a `Int32Vector`.
    * @returns {Int32Vector}
    */
    asInt32Vector() {
        var ret = wasm.vector_asFloat32Vector(this.ptr);
        return Int32Vector.__wrap(ret);
    }
    /**
    *Cast Vector as a `Int64Vector`.
    * @returns {Int64Vector}
    */
    asInt64Vector() {
        var ret = wasm.vector_asFloat64Vector(this.ptr);
        return Int64Vector.__wrap(ret);
    }
    /**
    *Cast Vector as a `Uint8Vector`.
    * @returns {Uint8Vector}
    */
    asUint8Vector() {
        var ret = wasm.vector_asInt8Vector(this.ptr);
        return Uint8Vector.__wrap(ret);
    }
    /**
    *Cast Vector as a `Uint16Vector`.
    * @returns {Uint16Vector}
    */
    asUint16Vector() {
        var ret = wasm.vector_asInt16Vector(this.ptr);
        return Uint16Vector.__wrap(ret);
    }
    /**
    *Cast Vector as a `Uint32Vector`.
    * @returns {Uint32Vector}
    */
    asUint32Vector() {
        var ret = wasm.vector_asFloat32Vector(this.ptr);
        return Uint32Vector.__wrap(ret);
    }
    /**
    *Cast Vector as a `Uint64Vector`.
    * @returns {Uint64Vector}
    */
    asUint64Vector() {
        var ret = wasm.vector_asFloat64Vector(this.ptr);
        return Uint64Vector.__wrap(ret);
    }
    /**
    *Cast Vector as a `Float32Vector`.
    * @returns {Float32Vector}
    */
    asFloat32Vector() {
        var ret = wasm.vector_asFloat32Vector(this.ptr);
        return Float32Vector.__wrap(ret);
    }
    /**
    *Cast Vector as a `Float64Vector`.
    * @returns {Float64Vector}
    */
    asFloat64Vector() {
        var ret = wasm.vector_asFloat64Vector(this.ptr);
        return Float64Vector.__wrap(ret);
    }
    /**
    * Cast Vector as a `BooleanVector`.
    * @returns {BooleanVector}
    */
    asBooleanVector() {
        var ret = wasm.vector_asBooleanVector(this.ptr);
        return BooleanVector.__wrap(ret);
    }
    /**
    * Cast Vector as a `StringVector`.
    * @returns {StringVector}
    */
    asStringVector() {
        var ret = wasm.vector_asStringVector(this.ptr);
        return StringVector.__wrap(ret);
    }
}
/**
*/
export class WasmUint8Array {

    static __wrap(ptr) {
        const obj = Object.create(WasmUint8Array.prototype);
        obj.ptr = ptr;

        return obj;
    }

    free() {
        const ptr = this.ptr;
        this.ptr = 0;

        wasm.__wbg_wasmuint8array_free(ptr);
    }
    /**
    * @param {number} size
    */
    constructor(size) {
        var ret = wasm.wasmuint8array_new(size);
        return WasmUint8Array.__wrap(ret);
    }
    /**
    * @returns {Uint8Array}
    */
    get view() {
        var ret = wasm.wasmuint8array_view(this.ptr);
        return takeObject(ret);
    }
}

async function load(module, imports) {
    if (typeof Response === 'function' && module instanceof Response) {

        if (typeof WebAssembly.instantiateStreaming === 'function') {
            try {
                return await WebAssembly.instantiateStreaming(module, imports);

            } catch (e) {
                if (module.headers.get('Content-Type') != 'application/wasm') {
                    console.warn("`WebAssembly.instantiateStreaming` failed because your server does not serve wasm with `application/wasm` MIME type. Falling back to `WebAssembly.instantiate` which is slower. Original error:\n", e);

                } else {
                    throw e;
                }
            }
        }

        const bytes = await module.arrayBuffer();
        return await WebAssembly.instantiate(bytes, imports);

    } else {

        const instance = await WebAssembly.instantiate(module, imports);

        if (instance instanceof WebAssembly.Instance) {
            return { instance, module };

        } else {
            return instance;
        }
    }
}

async function init(input) {
    if (typeof input === 'undefined') {
        input = import.meta.url.replace(/\.js$/, '_bg.wasm');
    }
    const imports = {};
    imports.wbg = {};
    imports.wbg.__wbindgen_json_parse = function(arg0, arg1) {
        var ret = JSON.parse(getStringFromWasm0(arg0, arg1));
        return addHeapObject(ret);
    };
    imports.wbg.__wbindgen_json_serialize = function(arg0, arg1) {
        const obj = getObject(arg1);
        var ret = JSON.stringify(obj === undefined ? null : obj);
        var ptr0 = passStringToWasm0(ret, wasm.__wbindgen_malloc, wasm.__wbindgen_realloc);
        var len0 = WASM_VECTOR_LEN;
        getInt32Memory0()[arg0 / 4 + 1] = len0;
        getInt32Memory0()[arg0 / 4 + 0] = ptr0;
    };
    imports.wbg.__wbindgen_string_new = function(arg0, arg1) {
        var ret = getStringFromWasm0(arg0, arg1);
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_vector_new = function(arg0) {
        var ret = Vector.__wrap(arg0);
        return addHeapObject(ret);
    };
    imports.wbg.__wbindgen_object_drop_ref = function(arg0) {
        takeObject(arg0);
    };
    imports.wbg.__wbg_new_1abc33d4f9ba3e80 = function() {
        var ret = new Array();
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_push_44968dcdf4cfbb43 = function(arg0, arg1) {
        var ret = getObject(arg0).push(getObject(arg1));
        return ret;
    };
    imports.wbg.__wbg_buffer_bc64154385c04ac4 = function(arg0) {
        var ret = getObject(arg0).buffer;
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_newwithbyteoffsetandlength_955ddd0ce3f6f8f7 = function(arg0, arg1, arg2) {
        var ret = new Int8Array(getObject(arg0), arg1 >>> 0, arg2 >>> 0);
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_new_396717534638d251 = function(arg0) {
        var ret = new Int8Array(getObject(arg0));
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_newwithbyteoffsetandlength_4ac754dd0e4a9d36 = function(arg0, arg1, arg2) {
        var ret = new Int16Array(getObject(arg0), arg1 >>> 0, arg2 >>> 0);
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_new_82fd9bbed79f6672 = function(arg0) {
        var ret = new Int16Array(getObject(arg0));
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_newwithbyteoffsetandlength_14c58fd914c5e030 = function(arg0, arg1, arg2) {
        var ret = new Int32Array(getObject(arg0), arg1 >>> 0, arg2 >>> 0);
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_new_9aae23408b655f27 = function(arg0) {
        var ret = new Int32Array(getObject(arg0));
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_newwithbyteoffsetandlength_3c8748473807c7cf = function(arg0, arg1, arg2) {
        var ret = new Uint8Array(getObject(arg0), arg1 >>> 0, arg2 >>> 0);
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_new_22a33711cf65b661 = function(arg0) {
        var ret = new Uint8Array(getObject(arg0));
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_newwithbyteoffsetandlength_00c580aa4e676e36 = function(arg0, arg1, arg2) {
        var ret = new Uint16Array(getObject(arg0), arg1 >>> 0, arg2 >>> 0);
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_new_1a48cdb23432547a = function(arg0) {
        var ret = new Uint16Array(getObject(arg0));
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_newwithbyteoffsetandlength_4fec8b44f7ca5e63 = function(arg0, arg1, arg2) {
        var ret = new Uint32Array(getObject(arg0), arg1 >>> 0, arg2 >>> 0);
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_new_ba68e0a53657e6d1 = function(arg0) {
        var ret = new Uint32Array(getObject(arg0));
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_newwithbyteoffsetandlength_193d0d8755287921 = function(arg0, arg1, arg2) {
        var ret = new Float32Array(getObject(arg0), arg1 >>> 0, arg2 >>> 0);
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_new_48c5d6d65ec9a035 = function(arg0) {
        var ret = new Float32Array(getObject(arg0));
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_newwithbyteoffsetandlength_18b6cc80ad42cae3 = function(arg0, arg1, arg2) {
        var ret = new Float64Array(getObject(arg0), arg1 >>> 0, arg2 >>> 0);
        return addHeapObject(ret);
    };
    imports.wbg.__wbg_new_1c8fb7e88e75aa6b = function(arg0) {
        var ret = new Float64Array(getObject(arg0));
        return addHeapObject(ret);
    };
    imports.wbg.__wbindgen_throw = function(arg0, arg1) {
        throw new Error(getStringFromWasm0(arg0, arg1));
    };
    imports.wbg.__wbindgen_rethrow = function(arg0) {
        throw takeObject(arg0);
    };
    imports.wbg.__wbindgen_memory = function() {
        var ret = wasm.memory;
        return addHeapObject(ret);
    };

    if (typeof input === 'string' || (typeof Request === 'function' && input instanceof Request) || (typeof URL === 'function' && input instanceof URL)) {
        input = fetch(input);
    }

    const { instance, module } = await load(await input, imports);

    wasm = instance.exports;
    init.__wbindgen_wasm_module = module;

    return wasm;
}

export default init;

