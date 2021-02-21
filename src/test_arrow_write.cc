#include <arrow/array.h>
#include <arrow/builder.h>
#include <arrow/io/file.h>
#include <arrow/ipc/writer.h>
#include <arrow/table.h>

#include <iostream>
#include <unordered_map>

using namespace std;
using namespace arrow;

/// some boilerplate and utils
#define OK_OR_DIE(exp)                                                  \
if (!exp.ok()) { cerr << "dead at " << __LINE__ << endl; exit(1); }

shared_ptr<Table> make_table(
    const unordered_map<string, shared_ptr<ChunkedArray> > &columns)
{
  vector<shared_ptr<Field>> fields;
  vector<shared_ptr<ChunkedArray>> arrays;

  for (auto &pair: columns) {
    fields.push_back(field(pair.first, pair.second->type(), false));
    arrays.push_back(pair.second);
  }
  return Table::Make(schema(fields), arrays);
}

template <typename Type>
shared_ptr<ChunkedArray> make_chunked_array(
    const vector<typename TypeTraits<Type>::CType> &vec)
{
  NumericBuilder<Type> builder;
  for (auto &val: vec) {
    OK_OR_DIE(builder.Append(val));
  }
  return shared_ptr<ChunkedArray>(
      new ChunkedArray(
          vector<shared_ptr<Array>>({ builder.Finish().ValueOrDie() })));
}

int main(int argc, char **argv)
{
  shared_ptr<Table> t = make_table(
      { {"a", make_chunked_array<UInt32Type>({1, 2, 3, 4}) } });
        
  shared_ptr<io::FileOutputStream>
      file = io::FileOutputStream::Open("out.arrow").ValueOrDie();

  shared_ptr<ipc::RecordBatchWriter>
      writer = ipc::MakeFileWriter(file, t->schema()).ValueOrDie();

  TableBatchReader reader(*t);
  shared_ptr<RecordBatch> batch;
  OK_OR_DIE(reader.ReadNext(&batch));

  OK_OR_DIE(writer->WriteTable(*t));
  // This call will segfault.
  cerr << "Will call Close" << endl;
  OK_OR_DIE(writer->Close());
  OK_OR_DIE(file->Close());
  
  return 0;
}
