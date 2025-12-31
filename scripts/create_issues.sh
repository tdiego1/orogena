#!/bin/bash
#
# create_issues.sh - Create GitHub issues from a JSON specification file
#
# Usage: ./scripts/create_issues.sh <json_file> [--dry-run]
#
# The JSON file should contain an array of issues with the following structure:
# {
#   "phase": {
#     "name": "phase-0",
#     "description": "Phase 0 - Foundation Completion",
#     "color": "00FF00"
#   },
#   "issues": [
#     {
#       "title": "Issue title",
#       "body": "Issue description",
#       "labels": ["enhancement"]
#     }
#   ]
# }

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check dependencies
check_dependencies() {
    if ! command -v gh &> /dev/null; then
        echo -e "${RED}Error: GitHub CLI (gh) is not installed${NC}"
        echo "Install it from: https://cli.github.com/"
        exit 1
    fi

    if ! command -v jq &> /dev/null; then
        echo -e "${RED}Error: jq is not installed${NC}"
        echo "Install it with: sudo pacman -S jq (Arch) or sudo apt install jq (Ubuntu)"
        exit 1
    fi

    # Check if authenticated
    if ! gh auth status &> /dev/null; then
        echo -e "${RED}Error: Not authenticated with GitHub CLI${NC}"
        echo "Run: gh auth login"
        exit 1
    fi
}

# Print usage
usage() {
    echo "Usage: $0 <json_file> [--dry-run]"
    echo ""
    echo "Arguments:"
    echo "  json_file   Path to the JSON file containing issue specifications"
    echo "  --dry-run   Print what would be created without actually creating issues"
    echo ""
    echo "Example:"
    echo "  $0 scripts/issues/phase-0.json"
    echo "  $0 scripts/issues/phase-0.json --dry-run"
    exit 1
}

# Create label if it doesn't exist
ensure_label() {
    local name="$1"
    local description="$2"
    local color="$3"
    local dry_run="$4"

    # Check if label exists
    if gh label list --json name -q ".[].name" | grep -q "^${name}$"; then
        echo -e "${BLUE}Label '${name}' already exists${NC}"
        return 0
    fi

    if [ "$dry_run" = "true" ]; then
        echo -e "${YELLOW}[DRY-RUN] Would create label: ${name} (${description})${NC}"
    else
        echo -e "${GREEN}Creating label: ${name}${NC}"
        gh label create "$name" --description "$description" --color "$color"
    fi
}

# Create a single issue
create_issue() {
    local title="$1"
    local body="$2"
    local labels="$3"
    local dry_run="$4"

    # Check if issue with same title already exists
    existing=$(gh issue list --search "in:title \"${title}\"" --json title -q ".[].title" | grep -F "$title" || true)
    if [ -n "$existing" ]; then
        echo -e "${YELLOW}Issue already exists: ${title}${NC}"
        return 0
    fi

    if [ "$dry_run" = "true" ]; then
        echo -e "${YELLOW}[DRY-RUN] Would create issue: ${title}${NC}"
        echo -e "  Labels: ${labels}"
    else
        echo -e "${GREEN}Creating issue: ${title}${NC}"
        gh issue create --title "$title" --body "$body" --label "$labels"
    fi
}

# Main function
main() {
    local json_file=""
    local dry_run="false"

    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --dry-run)
                dry_run="true"
                shift
                ;;
            -h|--help)
                usage
                ;;
            *)
                if [ -z "$json_file" ]; then
                    json_file="$1"
                else
                    echo -e "${RED}Error: Unexpected argument: $1${NC}"
                    usage
                fi
                shift
                ;;
        esac
    done

    if [ -z "$json_file" ]; then
        echo -e "${RED}Error: No JSON file specified${NC}"
        usage
    fi

    if [ ! -f "$json_file" ]; then
        echo -e "${RED}Error: File not found: ${json_file}${NC}"
        exit 1
    fi

    check_dependencies

    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}GitHub Issue Creator${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""

    if [ "$dry_run" = "true" ]; then
        echo -e "${YELLOW}Running in DRY-RUN mode - no changes will be made${NC}"
        echo ""
    fi

    # Read phase info
    local phase_name=$(jq -r '.phase.name' "$json_file")
    local phase_description=$(jq -r '.phase.description' "$json_file")
    local phase_color=$(jq -r '.phase.color' "$json_file")

    echo -e "Phase: ${GREEN}${phase_name}${NC} - ${phase_description}"
    echo ""

    # Ensure phase label exists
    ensure_label "$phase_name" "$phase_description" "$phase_color" "$dry_run"
    echo ""

    # Count issues
    local issue_count=$(jq '.issues | length' "$json_file")
    echo -e "Creating ${GREEN}${issue_count}${NC} issues..."
    echo ""

    # Create each issue
    for i in $(seq 0 $((issue_count - 1))); do
        local title=$(jq -r ".issues[$i].title" "$json_file")
        local body=$(jq -r ".issues[$i].body" "$json_file")
        local labels=$(jq -r ".issues[$i].labels | join(\",\")" "$json_file")

        # Add phase label to all issues
        if [ -n "$labels" ]; then
            labels="${labels},${phase_name}"
        else
            labels="${phase_name}"
        fi

        create_issue "$title" "$body" "$labels" "$dry_run"
    done

    echo ""
    echo -e "${GREEN}========================================${NC}"
    if [ "$dry_run" = "true" ]; then
        echo -e "${YELLOW}DRY-RUN complete. Run without --dry-run to create issues.${NC}"
    else
        echo -e "${GREEN}Done! Created issues for ${phase_name}${NC}"
    fi
    echo -e "${GREEN}========================================${NC}"
}

main "$@"
