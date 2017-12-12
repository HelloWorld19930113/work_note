# datum 和 blob 的区别  

## 1. Blob 类型   
```proto
// 指定一个 Blob 的 shape (dimensions).
message BlobShape {
  repeated int64 dim = 1 [packed = true];
}

message BlobProto {
  optional BlobShape shape = 7;
  repeated float data = 5 [packed = true];
  repeated float diff = 6 [packed = true];
  repeated double double_data = 8 [packed = true];
  repeated double double_diff = 9 [packed = true];

  // 4维数据 -- 过时的. 新版本使用 "shape" 来代替.
  optional int32 num = 1 [default = 0];
  optional int32 channels = 2 [default = 0];
  optional int32 height = 3 [default = 0];
  optional int32 width = 4 [default = 0];
}

message BlobProtoVector {
  repeated BlobProto blobs = 1;
}
```
最后生成的变量定义为：   
```cpp
...
private:
  ::BlobShape* shape_;
  ::google::protobuf::RepeatedField< float > data_;
  mutable int _data_cached_byte_size_;
  ::google::protobuf::RepeatedField< float > diff_;
  mutable int _diff_cached_byte_size_;
  ::google::protobuf::RepeatedField< double > double_data_;
  mutable int _double_data_cached_byte_size_;
  ::google::protobuf::RepeatedField< double > double_diff_;
  mutable int _double_diff_cached_byte_size_;
  ::google::protobuf::int32 num_;
  ::google::protobuf::int32 channels_;
  ::google::protobuf::int32 height_;
  ::google::protobuf::int32 width_;
```

## 2. Datum 类型   
`Datum`的`proto`格式定义。    
```proto
message Datum {
  optional int32 channels = 1;
  optional int32 height = 2;
  optional int32 width = 3;

  // the actual image data, in bytes
  optional bytes data = 4;
  optional int32 label = 5;

  // Optionally, the datum could also hold float data.
  repeated float float_data = 6;
  
  // If true data contains an encoded image that need to be decoded
  optional bool encoded = 7 [default = false];
}
```
根据`Datum`的定义可以看出，图像数据的存储方式可以有两种——`bytes`和`float`类型。



