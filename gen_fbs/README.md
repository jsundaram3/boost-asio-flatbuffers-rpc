Project Structure

gen_fbs/
├─ CMakeLists.txt          # Main project CMake
├─ entities/               # Folder with all entity headers
│   ├─ reflect.h
│   ├─ entities.h
│   └─ other_entity.h
├─ generator/              # Folder for generator executable
│   ├─ CMakeLists.txt
│   └─ fbs_generator.cpp
├─ fbs_schemas/            # Output folder for .fbs files (generated)
└─ src/                    # Your main project source
    └─ main.cpp