import init, * as arrow from "./pkg/arrow_wasm.js";

function vectorToArray(vec, type)
{
  if (type.name === 'floatingpoint' && type.precision === 'DOUBLE') {
    return vec.asFloat64Vector().toArray();
  } else if (type.name === 'floatingpoint' && type.precision === 'SINGLE') {
    return vec.asFloat32Vector().toArray();
  } else if (type.name === 'int' && type.bitWidth === 64 && type.isSigned) {
    return vec.asInt64Vector().toArray();
  } else if (type.name === 'int' && type.bitWidth === 64 && !type.isSigned) {
    return vec.asUint64Vector().toArray();
  } else if (type.name === 'int' && type.bitWidth === 32 && type.isSigned) {
    return vec.asInt32Vector().toArray();
  } else if (type.name === 'int' && type.bitWidth === 32 && !type.isSigned) {
    return vec.asUint32Vector().toArray();
  } else {
    throw new Error(`Unrecognized type: ${JSON.stringify(type)}`);
  }
}

function tableToDictOfArrays(table)
{
  let schema = table.schema.toJSON();
  let result = {};
  schema.fields.forEach((field, i) => {
    let name = field.name;
    let chunks = [];
    for (let batch = 0; batch < table.numBatches; ++batch) {
      chunks.push(vectorToArray(table.recordBatch(batch).column(i),
                                schema.fields[i].type));
    }
    result[name] = chunks;
  });
  return result;
}

async function run() {
  await init();
  
  const url = "/agg.arrow";
  
  const response = await fetch(url);
  const data = await response.arrayBuffer();
  const contents = new Uint8Array(data);
  
  let table = arrow.Table.from(contents);
  console.log(table.schema.toJSON());
  table = tableToDictOfArrays(table);
  
  debugger;
}

run();
