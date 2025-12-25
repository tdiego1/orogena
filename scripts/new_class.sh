#!/bin/bash
# Quick script to create new class from template

COMPONENT="$1"     # e.g., "global", "ui", "regional"
CLASS_NAME="$2"    # e.g., "PlatePhysics", "SimulationControls"

if [ -z "$COMPONENT" ] || [ -z "$CLASS_NAME" ]; then
    echo "Usage: $0 <component> <ClassName>"
    echo "Example: $0 global PlatePhysics"
    echo ""
    echo "Components: global, regional, local, rendering, ui, database, core, utils"
    exit 1
fi

# Convert PascalCase to snake_case for filename
LOWERCASE=$(echo "$CLASS_NAME" | sed 's/\([A-Z]\)/_\L\1/g' | sed 's/^_//')
FILENAME="${COMPONENT}_${LOWERCASE}"

# Capitalize first letter of component for namespace
NAMESPACE=$(echo "$COMPONENT" | sed 's/^\(.\)/\U\1/')

# Check if files already exist
if [ -f "src/${COMPONENT}/${FILENAME}.h" ] || [ -f "src/${COMPONENT}/${FILENAME}.cpp" ]; then
    echo "Error: Files already exist!"
    echo "  src/${COMPONENT}/${FILENAME}.h"
    echo "  src/${COMPONENT}/${FILENAME}.cpp"
    exit 1
fi

# Check if component directory exists
if [ ! -d "src/${COMPONENT}" ]; then
    echo "Error: Component directory src/${COMPONENT}/ does not exist"
    exit 1
fi

# Copy templates
cp docs/templates/header_template.h "src/${COMPONENT}/${FILENAME}.h"
cp docs/templates/source_template.cpp "src/${COMPONENT}/${FILENAME}.cpp"

# Do basic replacements
sed -i "s/component_class_name/${FILENAME}/g" "src/${COMPONENT}/${FILENAME}.h"
sed -i "s/component_class_name/${FILENAME}/g" "src/${COMPONENT}/${FILENAME}.cpp"
sed -i "s/ComponentName/${NAMESPACE}/g" "src/${COMPONENT}/${FILENAME}.h"
sed -i "s/ComponentName/${NAMESPACE}/g" "src/${COMPONENT}/${FILENAME}.cpp"
sed -i "s/ClassName/${CLASS_NAME}/g" "src/${COMPONENT}/${FILENAME}.h"
sed -i "s/ClassName/${CLASS_NAME}/g" "src/${COMPONENT}/${FILENAME}.cpp"

echo "✓ Created: src/${COMPONENT}/${FILENAME}.h"
echo "✓ Created: src/${COMPONENT}/${FILENAME}.cpp"
echo ""
echo "Next steps:"
echo "  1. Edit files and update documentation"
echo "  2. Remove unused sections (protected, static members, etc.)"
echo "  3. Add to src/CMakeLists.txt:"
echo "     - Add ${FILENAME}.cpp to orogena_${COMPONENT} library"
echo "  4. Write tests in tests/unit/test_${LOWERCASE}.cpp"
echo ""
echo "Happy coding!"
