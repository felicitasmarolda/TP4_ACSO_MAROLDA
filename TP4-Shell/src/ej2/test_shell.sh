#!/bin/bash

# test_shell.sh - Script para probar el shell implementado
# Uso: ./test_shell.sh

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Contadores
PASSED=0
FAILED=0
TOTAL=0

# Función para mostrar resultados
print_result() {
    local test_name="$1"
    local expected="$2"
    local actual="$3"
    local status="$4"
    
    echo -e "${BLUE}Test: ${test_name}${NC}"
    
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}[PASS]${NC}"
        ((PASSED++))
    else
        echo -e "${RED}[FAIL]${NC}"
        echo -e "${YELLOW}Expected:${NC}"
        echo "$expected"
        echo -e "${YELLOW}Actual:${NC}"
        echo "$actual"
        echo -e "${YELLOW}---${NC}"
        ((FAILED++))
    fi
    ((TOTAL++))
    echo
}

# Función para ejecutar test
run_test() {
    local test_name="$1"
    local shell_command="$2"
    local bash_command="$3"
    
    # Si no se proporciona bash_command, usar el mismo que shell_command
    if [ -z "$bash_command" ]; then
        bash_command="$shell_command"
    fi
    
    echo -e "${BLUE}Running: ${test_name}${NC}"
    
    # Ejecutar con bash (referencia)
    expected=$(echo "$bash_command" | bash 2>/dev/null | sort)
    
    # Ejecutar con nuestro shell
    actual=$(echo -e "$shell_command\nexit" | timeout 5s ./shell 2>/dev/null | grep -v "Shell>" | sort)
    
    # Comparar resultados
    if [ "$expected" = "$actual" ]; then
        print_result "$test_name" "$expected" "$actual" "PASS"
    else
        print_result "$test_name" "$expected" "$actual" "FAIL"
    fi
}

# Verificar que existe el ejecutable
if [ ! -f "./shell" ]; then
    echo -e "${RED}Error: No se encuentra el ejecutable './shell'${NC}"
    echo "Asegúrate de compilar primero con: make"
    exit 1
fi

echo -e "${BLUE}=== INICIO DE TESTS PARA EL SHELL ===${NC}"
echo

# Trabajar en el directorio actual (donde están los archivos reales)
echo -e "${YELLOW}=== TESTS BÁSICOS ===${NC}"

# Test 1: Comando simple sin pipes
run_test "Comando simple: ls" "ls" "ls"

# Test 2: Pipe básico - buscar archivos .c (que existen en tu proyecto)
run_test "Pipe básico: ls | grep .c" "ls | grep .c" "ls | grep .c"

# Test 3: Múltiples pipes - contar archivos .c
run_test "Múltiples pipes: ls | grep .c | wc -l" "ls | grep .c | wc -l" "ls | grep .c | wc -l"

# Test 4: Buscar archivos .sh (test_shell.sh existe)
run_test "Buscar .sh: ls | grep .sh" "ls | grep .sh" "ls | grep .sh"

# Test 5: Comando con argumentos - buscar archivos shell
run_test "ls con argumentos: ls -la | grep shell" "ls -la | grep shell" "ls -la | grep shell"

echo -e "${YELLOW}=== TESTS DE ROBUSTEZ ===${NC}"

# Test 6: Espacios extra alrededor de pipes
run_test "Espacios extra: ls | grep .c" "ls  |  grep .c" "ls | grep .c"

# Test 7: Comando más complejo - excluir directorios y tomar primeros 5
run_test "Comando complejo: ls -la | grep -v ^d | head -5" "ls -la | grep -v ^d | head -5" "ls -la | grep -v ^d | head -5"

# Test 8: Buscar archivos .o (shell.o existe)
run_test "Buscar .o: ls | grep .o" "ls | grep .o" "ls | grep .o"

# Test 9: wc (word count) - contar todos los archivos
run_test "Contar líneas: ls | wc -l" "ls | wc -l" "ls | wc -l"

# Test 10: sort
run_test "Ordenar: ls | sort" "ls | sort" "ls | sort"

echo -e "${YELLOW}=== TESTS ADICIONALES ===${NC}"

# Test 11: Buscar archivos que contengan "shell" en el nombre
run_test "Buscar 'shell': ls | grep shell" "ls | grep shell" "ls | grep shell"

# Test 12: Múltiples filtros - buscar archivos regulares que contengan "shell"
run_test "Múltiples filtros: ls -la | grep -v ^d | grep shell" "ls -la | grep -v ^d | grep shell" "ls -la | grep -v ^d | grep shell"

# Test 13: echo y grep - test sintético
run_test "echo pipe grep: echo 'shell.c Makefile test_shell.sh' | grep shell" "echo 'shell.c Makefile test_shell.sh' | grep shell" "echo 'shell.c Makefile test_shell.sh' | grep shell"

# Test 14: Buscar por extensión específica usando ls y grep
run_test "Buscar Makefile: ls | grep Makefile" "ls | grep Makefile" "ls | grep Makefile"

# Test 15: Filtro más específico - archivos que empiecen con 's'
run_test "Archivos con 's': ls | grep '^s'" "ls | grep '^s'" "ls | grep '^s'"

echo -e "${BLUE}=== RESUMEN DE TESTS ===${NC}"
echo -e "Total de tests: ${TOTAL}"
echo -e "${GREEN}Pasaron: ${PASSED}${NC}"
echo -e "${RED}Fallaron: ${FAILED}${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}¡Todos los tests pasaron! ✓${NC}"
    exit 0
else
    echo -e "${RED}Algunos tests fallaron. Revisa la implementación.${NC}"
    exit 1
fi