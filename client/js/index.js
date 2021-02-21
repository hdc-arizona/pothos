import init, * as arrow from "./pkg/arrow_wasm.js";

async function run() {
  await init();
  
  const url = "/agg.arrow";
  
  const response = await fetch(url);
  const data = await response.arrayBuffer();
  const contents = new Uint8Array(data);
  
  const table = arrow.Table.from(contents);
  console.log(table.schema.toJSON());
}

run();
