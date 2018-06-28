namespace cpp VersionChecker

struct Version{
1: i32 version,
2: i32 _major,
3: i32 _minor,
4: i32 additional
}


service VersionChecker {

Version  fetchVersion(1:Version ver),
void getFile()

}
