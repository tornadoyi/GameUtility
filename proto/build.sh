#!/bin/sh
# sed  -i '' 's|import "github.com/gogo/protobuf/gogoproto/gogo.proto";||g' pf.proto




cp pf.proto pf.proto.bak
sed -i '' -e '4i \ 
	import "github.com/gogo/protobuf/gogoproto/gogo.proto";' pf.proto
sed -i '' -e '5i \ 
	option (gogoproto.marshaler_all) = true;' pf.proto
sed -i '' -e '6i \ 
	option (gogoproto.sizer_all) = true;' pf.proto

sed -i '' -e '7i \ 
	option (gogoproto.unmarshaler_all) = true;' pf.proto

protoc --proto_path=$GOPATH/src/github.com/gogo/protobuf/protobuf:../../../:. --gogo_out=.   pf.proto
mv pf.pb.go ../pf/pf.pb.go
go install zerogame.info/tapalliance/pf

mv -f pf.proto.bak pf.proto