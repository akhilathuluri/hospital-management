#!/bin/bash
# Compilation script for MSYS2

echo "Compiling Hospital Management System..."
make clean
make

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Compilation successful!"
    echo ""
    echo "To run the application:"
    echo "  ./hospital_management.exe"
    echo ""
    echo "Default login: admin / admin123"
else
    echo ""
    echo "❌ Compilation failed. Please check the errors above."
fi
