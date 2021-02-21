/* tslint:disable */
/* eslint-disable */
/**
*/
export class ArrowVectorFFI {
  free(): void;
/**
* @returns {number}
*/
  array: number;
/**
* @returns {number}
*/
  schema: number;
}
/**
*/
export class BooleanVector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {boolean}
*/
  get(index: number): boolean;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {BooleanVector}
*/
  slice(offset: number, length: number): BooleanVector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {BooleanVector}
*/
  limit(num_elements: number): BooleanVector;
/**
* @param {Uint8Array} data
* @param {number} length
* @returns {BooleanVector}
*/
  static from(data: Uint8Array, length: number): BooleanVector;
/**
* Returns a `Buffer` holding all the values of this array.
*
* Note this doesn't take the offset of this array into account.
* @returns {Uint8Array}
*/
  toArray(): Uint8Array;
/**
* Creates a JS typed array which is a view into wasm's linear memory at the slice specified.
* This function returns a new typed array which is a view into wasm's memory. This view does not copy the underlying data.
* @returns {Uint8Array}
*/
  view(): Uint8Array;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class DataType {
  free(): void;
/**
* Generate a JSON representation.
* @returns {any}
*/
  toJSON(): any;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Parse a `Field` definition from a JSON representation.
* @param {any} json
* @returns {DataType}
*/
  static from(json: any): DataType;
}
/**
*/
export class Field {
  free(): void;
/**
* Generate a JSON representation.
* @returns {any}
*/
  toJSON(): any;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Parse a `Field` definition from a JSON representation.
* @param {any} json
* @returns {Field}
*/
  static from(json: any): Field;
/**
* @returns {DataType}
*/
  readonly dataType: DataType;
/**
* @returns {BigInt | undefined}
*/
  readonly dictId: BigInt | undefined;
/**
* @returns {boolean | undefined}
*/
  readonly dictIsOrdered: boolean | undefined;
/**
* @returns {boolean}
*/
  readonly isNullable: boolean;
/**
* @returns {string}
*/
  readonly name: string;
}
/**
*/
export class Float32Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {number}
*/
  get(index: number): number;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Float32Vector}
*/
  slice(offset: number, length: number): Float32Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Float32Vector}
*/
  limit(num_elements: number): Float32Vector;
/**
* @param {Float32Array} data
* @returns {Float32Vector}
*/
  static from(data: Float32Array): Float32Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {number | undefined}
*/
  sum(): number | undefined;
/**
* @returns {number | undefined}
*/
  min(): number | undefined;
/**
* @returns {number | undefined}
*/
  max(): number | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {Float32Array}
*/
  toArray(): Float32Array;
/**
* @returns {Float32Array}
*/
  view(): Float32Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Float64Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {number}
*/
  get(index: number): number;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Float64Vector}
*/
  slice(offset: number, length: number): Float64Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Float64Vector}
*/
  limit(num_elements: number): Float64Vector;
/**
* @param {Float64Array} data
* @returns {Float64Vector}
*/
  static from(data: Float64Array): Float64Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {number | undefined}
*/
  sum(): number | undefined;
/**
* @returns {number | undefined}
*/
  min(): number | undefined;
/**
* @returns {number | undefined}
*/
  max(): number | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {Float64Array}
*/
  toArray(): Float64Array;
/**
* @returns {Float64Array}
*/
  view(): Float64Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Int16Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {number}
*/
  get(index: number): number;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Int16Vector}
*/
  slice(offset: number, length: number): Int16Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Int16Vector}
*/
  limit(num_elements: number): Int16Vector;
/**
* @param {Int16Array} data
* @returns {Int16Vector}
*/
  static from(data: Int16Array): Int16Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {number | undefined}
*/
  sum(): number | undefined;
/**
* @returns {number | undefined}
*/
  min(): number | undefined;
/**
* @returns {number | undefined}
*/
  max(): number | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {Int16Array}
*/
  toArray(): Int16Array;
/**
* @returns {Int16Array}
*/
  view(): Int16Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Int32Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {number}
*/
  get(index: number): number;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Int32Vector}
*/
  slice(offset: number, length: number): Int32Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Int32Vector}
*/
  limit(num_elements: number): Int32Vector;
/**
* @param {Int32Array} data
* @returns {Int32Vector}
*/
  static from(data: Int32Array): Int32Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {number | undefined}
*/
  sum(): number | undefined;
/**
* @returns {number | undefined}
*/
  min(): number | undefined;
/**
* @returns {number | undefined}
*/
  max(): number | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {Int32Array}
*/
  toArray(): Int32Array;
/**
* @returns {Int32Array}
*/
  view(): Int32Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Int64Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {BigInt}
*/
  get(index: number): BigInt;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Int64Vector}
*/
  slice(offset: number, length: number): Int64Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Int64Vector}
*/
  limit(num_elements: number): Int64Vector;
/**
* @param {BigInt64Array} data
* @returns {Int64Vector}
*/
  static from(data: BigInt64Array): Int64Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {BigInt | undefined}
*/
  sum(): BigInt | undefined;
/**
* @returns {BigInt | undefined}
*/
  min(): BigInt | undefined;
/**
* @returns {BigInt | undefined}
*/
  max(): BigInt | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {BigInt64Array}
*/
  toArray(): BigInt64Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Int8Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {number}
*/
  get(index: number): number;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Int8Vector}
*/
  slice(offset: number, length: number): Int8Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Int8Vector}
*/
  limit(num_elements: number): Int8Vector;
/**
* @param {Int8Array} data
* @returns {Int8Vector}
*/
  static from(data: Int8Array): Int8Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {number | undefined}
*/
  sum(): number | undefined;
/**
* @returns {number | undefined}
*/
  min(): number | undefined;
/**
* @returns {number | undefined}
*/
  max(): number | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {Int8Array}
*/
  toArray(): Int8Array;
/**
* @returns {Int8Array}
*/
  view(): Int8Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class RecordBatch {
  free(): void;
/**
* Get a column's vector by index.
* @param {number} index
* @returns {Vector}
*/
  column(index: number): Vector;
/**
* Get a column's vector by name.
* @param {string} name
* @returns {Vector}
*/
  columnWithName(name: string): Vector;
/**
* Get all columns in the record batch.
* @returns {Array<any>}
*/
  readonly columns: Array<any>;
/**
* @returns {number}
*/
  readonly numColumns: number;
/**
* @returns {number}
*/
  readonly numRows: number;
/**
* Returns the schema of the record batches.
* @returns {Schema}
*/
  readonly schema: Schema;
}
/**
*/
export class Schema {
  free(): void;
/**
* Generate a JSON representation.
* @returns {any}
*/
  toJSON(): any;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* @param {number} i
* @returns {Field}
*/
  field(i: number): Field;
/**
* Look up a column by name and return a immutable reference to the column along with its index.
* @param {string} name
* @returns {any}
*/
  columnWithName(name: string): any;
/**
* Find the index of the column with the given name.
* @param {string} name
* @returns {number}
*/
  indexOf(name: string): number;
/**
* @param {string} name
* @returns {Field}
*/
  fieldWithName(name: string): Field;
/**
* Parse a `Schema` definition from a JSON representation.
* @param {any} json
* @returns {Schema}
*/
  static from(json: any): Schema;
/**
* @returns {any}
*/
  readonly fields: any;
/**
* Returns custom metadata key-value pairs.
* @returns {any}
*/
  readonly metadata: any;
/**
* @returns {number}
*/
  readonly numFields: number;
}
/**
* String vector
*/
export class StringVector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {string}
*/
  get(index: number): string;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {StringVector}
*/
  slice(offset: number, length: number): StringVector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {StringVector}
*/
  limit(num_elements: number): StringVector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Table {
  free(): void;
/**
* @param {number} index
* @returns {RecordBatch | undefined}
*/
  recordBatch(index: number): RecordBatch | undefined;
/**
* Create a table from IPC bytes. Use `fromWasmUint8Array` to avoid memory copies.
* @param {Uint8Array} contents
* @returns {Table}
*/
  static from(contents: Uint8Array): Table;
/**
* Create a table from a pre-initialized buffer. The memory is passed without a copy.
* @param {WasmUint8Array} data
* @returns {Table}
*/
  static fromWasmUint8Array(data: WasmUint8Array): Table;
/**
* @returns {Uint8Array}
*/
  serialize(): Uint8Array;
/**
* Return the number of batches in the file
* @returns {number}
*/
  readonly numBatches: number;
/**
* Returns the schema of the record batches.
* @returns {Schema}
*/
  readonly schema: Schema;
}
/**
*/
export class Uint16Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {number}
*/
  get(index: number): number;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Uint16Vector}
*/
  slice(offset: number, length: number): Uint16Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Uint16Vector}
*/
  limit(num_elements: number): Uint16Vector;
/**
* @param {Uint16Array} data
* @returns {Uint16Vector}
*/
  static from(data: Uint16Array): Uint16Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {number | undefined}
*/
  sum(): number | undefined;
/**
* @returns {number | undefined}
*/
  min(): number | undefined;
/**
* @returns {number | undefined}
*/
  max(): number | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {Uint16Array}
*/
  toArray(): Uint16Array;
/**
* @returns {Uint16Array}
*/
  view(): Uint16Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Uint32Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {number}
*/
  get(index: number): number;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Uint32Vector}
*/
  slice(offset: number, length: number): Uint32Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Uint32Vector}
*/
  limit(num_elements: number): Uint32Vector;
/**
* @param {Uint32Array} data
* @returns {Uint32Vector}
*/
  static from(data: Uint32Array): Uint32Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {number | undefined}
*/
  sum(): number | undefined;
/**
* @returns {number | undefined}
*/
  min(): number | undefined;
/**
* @returns {number | undefined}
*/
  max(): number | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {Uint32Array}
*/
  toArray(): Uint32Array;
/**
* @returns {Uint32Array}
*/
  view(): Uint32Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Uint64Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {BigInt}
*/
  get(index: number): BigInt;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Uint64Vector}
*/
  slice(offset: number, length: number): Uint64Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Uint64Vector}
*/
  limit(num_elements: number): Uint64Vector;
/**
* @param {BigUint64Array} data
* @returns {Uint64Vector}
*/
  static from(data: BigUint64Array): Uint64Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {BigInt | undefined}
*/
  sum(): BigInt | undefined;
/**
* @returns {BigInt | undefined}
*/
  min(): BigInt | undefined;
/**
* @returns {BigInt | undefined}
*/
  max(): BigInt | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {BigUint64Array}
*/
  toArray(): BigUint64Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Uint8Vector {
  free(): void;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
* Returns the primitive value at `index`.
* @param {number} index
* @returns {number}
*/
  get(index: number): number;
/**
* Returns a zero-copy slice of this array with the indicated offset and length.
* @param {number} offset
* @param {number} length
* @returns {Uint8Vector}
*/
  slice(offset: number, length: number): Uint8Vector;
/**
* Returns the array, taking only the number of elements specified.
* @param {number} num_elements
* @returns {Uint8Vector}
*/
  limit(num_elements: number): Uint8Vector;
/**
* @param {Uint8Array} data
* @returns {Uint8Vector}
*/
  static from(data: Uint8Array): Uint8Vector;
/**
* Returns the contents of the vector as a JSON array.
* @returns {any}
*/
  toJSON(): any;
/**
* @returns {number | undefined}
*/
  sum(): number | undefined;
/**
* @returns {number | undefined}
*/
  min(): number | undefined;
/**
* @returns {number | undefined}
*/
  max(): number | undefined;
/**
* Returns the contents of the vector as a typed array.
* @returns {Uint8Array}
*/
  toArray(): Uint8Array;
/**
* @returns {Uint8Array}
*/
  view(): Uint8Array;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class Vector {
  free(): void;
/**
* Make a vector from binary in the C Data Interface (FFI).
* @param {number} array
* @param {number} schema
* @returns {Vector}
*/
  static fromRaw(array: number, schema: number): Vector;
/**
* Generate a String representation.
* @returns {string}
*/
  toString(): string;
/**
* Returns whether the element at `index` is not null.
* @param {number} index
* @returns {boolean}
*/
  isValid(index: number): boolean;
/**
* Returns whether the element at `index` is null.
* @param {number} index
* @returns {boolean}
*/
  isNull(index: number): boolean;
/**
* Returns two pointers that represent this vector in the C Data Interface (FFI).
* @returns {ArrowVectorFFI}
*/
  toRaw(): ArrowVectorFFI;
/**
*Cast Vector as a `Int8Vector`.
* @returns {Int8Vector}
*/
  asInt8Vector(): Int8Vector;
/**
*Cast Vector as a `Int16Vector`.
* @returns {Int16Vector}
*/
  asInt16Vector(): Int16Vector;
/**
*Cast Vector as a `Int32Vector`.
* @returns {Int32Vector}
*/
  asInt32Vector(): Int32Vector;
/**
*Cast Vector as a `Int64Vector`.
* @returns {Int64Vector}
*/
  asInt64Vector(): Int64Vector;
/**
*Cast Vector as a `Uint8Vector`.
* @returns {Uint8Vector}
*/
  asUint8Vector(): Uint8Vector;
/**
*Cast Vector as a `Uint16Vector`.
* @returns {Uint16Vector}
*/
  asUint16Vector(): Uint16Vector;
/**
*Cast Vector as a `Uint32Vector`.
* @returns {Uint32Vector}
*/
  asUint32Vector(): Uint32Vector;
/**
*Cast Vector as a `Uint64Vector`.
* @returns {Uint64Vector}
*/
  asUint64Vector(): Uint64Vector;
/**
*Cast Vector as a `Float32Vector`.
* @returns {Float32Vector}
*/
  asFloat32Vector(): Float32Vector;
/**
*Cast Vector as a `Float64Vector`.
* @returns {Float64Vector}
*/
  asFloat64Vector(): Float64Vector;
/**
* Cast Vector as a `BooleanVector`.
* @returns {BooleanVector}
*/
  asBooleanVector(): BooleanVector;
/**
* Cast Vector as a `StringVector`.
* @returns {StringVector}
*/
  asStringVector(): StringVector;
/**
* Returns whether this vector is empty.
* @returns {boolean}
*/
  readonly isEmpty: boolean;
/**
* Get the length of the vector.
* @returns {number}
*/
  readonly length: number;
/**
* Returns the total number of null values in this vector.
* @returns {number}
*/
  readonly nullCount: number;
}
/**
*/
export class WasmUint8Array {
  free(): void;
/**
* @param {number} size
*/
  constructor(size: number);
/**
* @returns {Uint8Array}
*/
  readonly view: Uint8Array;
}

export type InitInput = RequestInfo | URL | Response | BufferSource | WebAssembly.Module;

export interface InitOutput {
  readonly memory: WebAssembly.Memory;
  readonly __wbg_datatype_free: (a: number) => void;
  readonly datatype_toJSON: (a: number) => number;
  readonly datatype_toString: (a: number, b: number) => void;
  readonly datatype_from: (a: number) => number;
  readonly __wbg_field_free: (a: number) => void;
  readonly field_toJSON: (a: number) => number;
  readonly field_toString: (a: number, b: number) => void;
  readonly field_name: (a: number, b: number) => void;
  readonly field_isNullable: (a: number) => number;
  readonly field_dictId: (a: number, b: number) => void;
  readonly field_dictIsOrdered: (a: number) => number;
  readonly field_dataType: (a: number) => number;
  readonly field_from: (a: number) => number;
  readonly __wbg_recordbatch_free: (a: number) => void;
  readonly recordbatch_numRows: (a: number) => number;
  readonly recordbatch_numColumns: (a: number) => number;
  readonly recordbatch_schema: (a: number) => number;
  readonly recordbatch_column: (a: number, b: number) => number;
  readonly recordbatch_columns: (a: number) => number;
  readonly recordbatch_columnWithName: (a: number, b: number, c: number) => number;
  readonly __wbg_schema_free: (a: number) => void;
  readonly schema_toJSON: (a: number) => number;
  readonly schema_toString: (a: number, b: number) => void;
  readonly schema_field: (a: number, b: number) => number;
  readonly schema_fields: (a: number) => number;
  readonly schema_numFields: (a: number) => number;
  readonly schema_columnWithName: (a: number, b: number, c: number) => number;
  readonly schema_indexOf: (a: number, b: number, c: number) => number;
  readonly schema_fieldWithName: (a: number, b: number, c: number) => number;
  readonly schema_metadata: (a: number) => number;
  readonly schema_from: (a: number) => number;
  readonly __wbg_table_free: (a: number) => void;
  readonly table_schema: (a: number) => number;
  readonly table_recordBatch: (a: number, b: number) => number;
  readonly table_numBatches: (a: number) => number;
  readonly table_from: (a: number, b: number) => number;
  readonly table_fromWasmUint8Array: (a: number) => number;
  readonly table_serialize: (a: number) => number;
  readonly __wbg_wasmuint8array_free: (a: number) => void;
  readonly wasmuint8array_new: (a: number) => number;
  readonly wasmuint8array_view: (a: number) => number;
  readonly __wbg_arrowvectorffi_free: (a: number) => void;
  readonly __wbg_get_arrowvectorffi_array: (a: number) => number;
  readonly __wbg_set_arrowvectorffi_array: (a: number, b: number) => void;
  readonly __wbg_get_arrowvectorffi_schema: (a: number) => number;
  readonly __wbg_set_arrowvectorffi_schema: (a: number, b: number) => void;
  readonly __wbg_vector_free: (a: number) => void;
  readonly vector_fromRaw: (a: number, b: number) => number;
  readonly vector_toString: (a: number, b: number) => void;
  readonly vector_length: (a: number) => number;
  readonly vector_isValid: (a: number, b: number) => number;
  readonly vector_isNull: (a: number, b: number) => number;
  readonly vector_isEmpty: (a: number) => number;
  readonly vector_nullCount: (a: number) => number;
  readonly vector_toRaw: (a: number) => number;
  readonly int8vector_toString: (a: number, b: number) => void;
  readonly int8vector_toRaw: (a: number) => number;
  readonly int8vector_get: (a: number, b: number) => number;
  readonly int8vector_slice: (a: number, b: number, c: number) => number;
  readonly int8vector_limit: (a: number, b: number) => number;
  readonly int8vector_from: (a: number, b: number) => number;
  readonly int8vector_toJSON: (a: number) => number;
  readonly int8vector_sum: (a: number) => number;
  readonly int8vector_min: (a: number) => number;
  readonly int8vector_max: (a: number) => number;
  readonly vector_asInt8Vector: (a: number) => number;
  readonly int8vector_toArray: (a: number) => number;
  readonly int8vector_view: (a: number) => number;
  readonly int16vector_toString: (a: number, b: number) => void;
  readonly int16vector_get: (a: number, b: number) => number;
  readonly int16vector_slice: (a: number, b: number, c: number) => number;
  readonly int16vector_limit: (a: number, b: number) => number;
  readonly int16vector_from: (a: number, b: number) => number;
  readonly int16vector_toJSON: (a: number) => number;
  readonly int16vector_sum: (a: number) => number;
  readonly int16vector_min: (a: number) => number;
  readonly int16vector_max: (a: number) => number;
  readonly vector_asInt16Vector: (a: number) => number;
  readonly int16vector_toArray: (a: number) => number;
  readonly int16vector_view: (a: number) => number;
  readonly int32vector_toString: (a: number, b: number) => void;
  readonly int32vector_get: (a: number, b: number) => number;
  readonly int32vector_from: (a: number, b: number) => number;
  readonly int32vector_toJSON: (a: number) => number;
  readonly int32vector_sum: (a: number, b: number) => void;
  readonly int32vector_min: (a: number, b: number) => void;
  readonly int32vector_max: (a: number, b: number) => void;
  readonly int32vector_toArray: (a: number) => number;
  readonly int32vector_view: (a: number) => number;
  readonly int64vector_toString: (a: number, b: number) => void;
  readonly int64vector_get: (a: number, b: number, c: number) => void;
  readonly int64vector_from: (a: number, b: number) => number;
  readonly int64vector_toJSON: (a: number) => number;
  readonly int64vector_sum: (a: number, b: number) => void;
  readonly int64vector_min: (a: number, b: number) => void;
  readonly int64vector_max: (a: number, b: number) => void;
  readonly int64vector_toArray: (a: number, b: number) => void;
  readonly uint8vector_toString: (a: number, b: number) => void;
  readonly uint8vector_get: (a: number, b: number) => number;
  readonly uint8vector_from: (a: number, b: number) => number;
  readonly uint8vector_toJSON: (a: number) => number;
  readonly uint8vector_sum: (a: number) => number;
  readonly uint8vector_min: (a: number) => number;
  readonly uint8vector_max: (a: number) => number;
  readonly uint8vector_toArray: (a: number) => number;
  readonly uint8vector_view: (a: number) => number;
  readonly uint16vector_toString: (a: number, b: number) => void;
  readonly uint16vector_get: (a: number, b: number) => number;
  readonly uint16vector_from: (a: number, b: number) => number;
  readonly uint16vector_toJSON: (a: number) => number;
  readonly uint16vector_sum: (a: number) => number;
  readonly uint16vector_min: (a: number) => number;
  readonly uint16vector_max: (a: number) => number;
  readonly uint16vector_toArray: (a: number) => number;
  readonly uint16vector_view: (a: number) => number;
  readonly uint32vector_toString: (a: number, b: number) => void;
  readonly uint32vector_from: (a: number, b: number) => number;
  readonly uint32vector_toJSON: (a: number) => number;
  readonly uint32vector_min: (a: number, b: number) => void;
  readonly uint32vector_max: (a: number, b: number) => void;
  readonly uint32vector_toArray: (a: number) => number;
  readonly uint32vector_view: (a: number) => number;
  readonly uint64vector_toString: (a: number, b: number) => void;
  readonly uint64vector_from: (a: number, b: number) => number;
  readonly uint64vector_toJSON: (a: number) => number;
  readonly uint64vector_min: (a: number, b: number) => void;
  readonly uint64vector_max: (a: number, b: number) => void;
  readonly float32vector_toString: (a: number, b: number) => void;
  readonly float32vector_length: (a: number) => number;
  readonly float32vector_isValid: (a: number, b: number) => number;
  readonly float32vector_isNull: (a: number, b: number) => number;
  readonly float32vector_isEmpty: (a: number) => number;
  readonly float32vector_nullCount: (a: number) => number;
  readonly float32vector_toRaw: (a: number) => number;
  readonly float32vector_get: (a: number, b: number) => number;
  readonly float32vector_slice: (a: number, b: number, c: number) => number;
  readonly float32vector_limit: (a: number, b: number) => number;
  readonly float32vector_from: (a: number, b: number) => number;
  readonly float32vector_toJSON: (a: number) => number;
  readonly float32vector_sum: (a: number, b: number) => void;
  readonly float32vector_min: (a: number, b: number) => void;
  readonly float32vector_max: (a: number, b: number) => void;
  readonly vector_asFloat32Vector: (a: number) => number;
  readonly float32vector_toArray: (a: number) => number;
  readonly float32vector_view: (a: number) => number;
  readonly float64vector_toString: (a: number, b: number) => void;
  readonly float64vector_get: (a: number, b: number) => number;
  readonly float64vector_slice: (a: number, b: number, c: number) => number;
  readonly float64vector_limit: (a: number, b: number) => number;
  readonly float64vector_from: (a: number, b: number) => number;
  readonly float64vector_toJSON: (a: number) => number;
  readonly float64vector_sum: (a: number, b: number) => void;
  readonly float64vector_min: (a: number, b: number) => void;
  readonly float64vector_max: (a: number, b: number) => void;
  readonly vector_asFloat64Vector: (a: number) => number;
  readonly float64vector_toArray: (a: number) => number;
  readonly float64vector_view: (a: number) => number;
  readonly __wbg_booleanvector_free: (a: number) => void;
  readonly booleanvector_toString: (a: number, b: number) => void;
  readonly booleanvector_length: (a: number) => number;
  readonly booleanvector_isValid: (a: number, b: number) => number;
  readonly booleanvector_isNull: (a: number, b: number) => number;
  readonly booleanvector_isEmpty: (a: number) => number;
  readonly booleanvector_nullCount: (a: number) => number;
  readonly booleanvector_toRaw: (a: number) => number;
  readonly booleanvector_get: (a: number, b: number) => number;
  readonly booleanvector_slice: (a: number, b: number, c: number) => number;
  readonly booleanvector_limit: (a: number, b: number) => number;
  readonly booleanvector_from: (a: number, b: number, c: number) => number;
  readonly booleanvector_toArray: (a: number, b: number) => void;
  readonly booleanvector_view: (a: number) => number;
  readonly booleanvector_toJSON: (a: number) => number;
  readonly vector_asBooleanVector: (a: number) => number;
  readonly __wbg_stringvector_free: (a: number) => void;
  readonly stringvector_toString: (a: number, b: number) => void;
  readonly stringvector_length: (a: number) => number;
  readonly stringvector_isValid: (a: number, b: number) => number;
  readonly stringvector_isNull: (a: number, b: number) => number;
  readonly stringvector_isEmpty: (a: number) => number;
  readonly stringvector_nullCount: (a: number) => number;
  readonly stringvector_toRaw: (a: number) => number;
  readonly stringvector_get: (a: number, b: number, c: number) => void;
  readonly stringvector_slice: (a: number, b: number, c: number) => number;
  readonly stringvector_limit: (a: number, b: number) => number;
  readonly stringvector_toJSON: (a: number) => number;
  readonly vector_asStringVector: (a: number) => number;
  readonly __wbg_int8vector_free: (a: number) => void;
  readonly __wbg_int32vector_free: (a: number) => void;
  readonly __wbg_int64vector_free: (a: number) => void;
  readonly __wbg_uint8vector_free: (a: number) => void;
  readonly __wbg_uint16vector_free: (a: number) => void;
  readonly __wbg_uint32vector_free: (a: number) => void;
  readonly __wbg_uint64vector_free: (a: number) => void;
  readonly __wbg_int16vector_free: (a: number) => void;
  readonly __wbg_float64vector_free: (a: number) => void;
  readonly __wbg_float32vector_free: (a: number) => void;
  readonly int8vector_isValid: (a: number, b: number) => number;
  readonly int8vector_isNull: (a: number, b: number) => number;
  readonly int32vector_isValid: (a: number, b: number) => number;
  readonly int32vector_isNull: (a: number, b: number) => number;
  readonly int64vector_isValid: (a: number, b: number) => number;
  readonly int64vector_isNull: (a: number, b: number) => number;
  readonly uint8vector_isValid: (a: number, b: number) => number;
  readonly uint8vector_isNull: (a: number, b: number) => number;
  readonly uint16vector_isValid: (a: number, b: number) => number;
  readonly uint16vector_isNull: (a: number, b: number) => number;
  readonly uint32vector_isValid: (a: number, b: number) => number;
  readonly uint32vector_isNull: (a: number, b: number) => number;
  readonly uint64vector_isValid: (a: number, b: number) => number;
  readonly uint64vector_isNull: (a: number, b: number) => number;
  readonly int16vector_isValid: (a: number, b: number) => number;
  readonly int16vector_isNull: (a: number, b: number) => number;
  readonly float64vector_isValid: (a: number, b: number) => number;
  readonly float64vector_isNull: (a: number, b: number) => number;
  readonly int8vector_isEmpty: (a: number) => number;
  readonly int32vector_isEmpty: (a: number) => number;
  readonly int64vector_isEmpty: (a: number) => number;
  readonly uint8vector_isEmpty: (a: number) => number;
  readonly uint16vector_isEmpty: (a: number) => number;
  readonly uint32vector_isEmpty: (a: number) => number;
  readonly uint64vector_isEmpty: (a: number) => number;
  readonly int16vector_isEmpty: (a: number) => number;
  readonly float64vector_isEmpty: (a: number) => number;
  readonly uint64vector_sum: (a: number, b: number) => void;
  readonly uint64vector_get: (a: number, b: number, c: number) => void;
  readonly uint64vector_toArray: (a: number, b: number) => void;
  readonly uint8vector_limit: (a: number, b: number) => number;
  readonly uint16vector_limit: (a: number, b: number) => number;
  readonly uint32vector_limit: (a: number, b: number) => number;
  readonly uint64vector_limit: (a: number, b: number) => number;
  readonly int32vector_limit: (a: number, b: number) => number;
  readonly int64vector_limit: (a: number, b: number) => number;
  readonly uint32vector_get: (a: number, b: number) => number;
  readonly vector_asUint8Vector: (a: number) => number;
  readonly vector_asUint16Vector: (a: number) => number;
  readonly vector_asUint32Vector: (a: number) => number;
  readonly vector_asUint64Vector: (a: number) => number;
  readonly vector_asInt32Vector: (a: number) => number;
  readonly vector_asInt64Vector: (a: number) => number;
  readonly uint8vector_slice: (a: number, b: number, c: number) => number;
  readonly uint16vector_slice: (a: number, b: number, c: number) => number;
  readonly uint32vector_slice: (a: number, b: number, c: number) => number;
  readonly uint64vector_slice: (a: number, b: number, c: number) => number;
  readonly int32vector_slice: (a: number, b: number, c: number) => number;
  readonly int64vector_slice: (a: number, b: number, c: number) => number;
  readonly uint32vector_sum: (a: number, b: number) => void;
  readonly int8vector_length: (a: number) => number;
  readonly int8vector_nullCount: (a: number) => number;
  readonly int32vector_length: (a: number) => number;
  readonly int32vector_nullCount: (a: number) => number;
  readonly int64vector_length: (a: number) => number;
  readonly int64vector_nullCount: (a: number) => number;
  readonly uint8vector_length: (a: number) => number;
  readonly uint8vector_nullCount: (a: number) => number;
  readonly uint16vector_length: (a: number) => number;
  readonly uint16vector_nullCount: (a: number) => number;
  readonly uint32vector_length: (a: number) => number;
  readonly uint32vector_nullCount: (a: number) => number;
  readonly uint64vector_length: (a: number) => number;
  readonly uint64vector_nullCount: (a: number) => number;
  readonly int16vector_length: (a: number) => number;
  readonly int16vector_nullCount: (a: number) => number;
  readonly float64vector_length: (a: number) => number;
  readonly float64vector_nullCount: (a: number) => number;
  readonly int32vector_toRaw: (a: number) => number;
  readonly int64vector_toRaw: (a: number) => number;
  readonly uint8vector_toRaw: (a: number) => number;
  readonly uint16vector_toRaw: (a: number) => number;
  readonly uint32vector_toRaw: (a: number) => number;
  readonly uint64vector_toRaw: (a: number) => number;
  readonly int16vector_toRaw: (a: number) => number;
  readonly float64vector_toRaw: (a: number) => number;
  readonly __wbindgen_malloc: (a: number) => number;
  readonly __wbindgen_realloc: (a: number, b: number, c: number) => number;
  readonly __wbindgen_add_to_stack_pointer: (a: number) => number;
  readonly __wbindgen_free: (a: number, b: number) => void;
}

/**
* If `module_or_path` is {RequestInfo} or {URL}, makes a request and
* for everything else, calls `WebAssembly.instantiate` directly.
*
* @param {InitInput | Promise<InitInput>} module_or_path
*
* @returns {Promise<InitOutput>}
*/
export default function init (module_or_path?: InitInput | Promise<InitInput>): Promise<InitOutput>;
        