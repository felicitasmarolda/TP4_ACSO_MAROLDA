#!/bin/bash

# test_shell.sh - Script para probar el shell implementado (CORREGIDO)
# Enfocado en pipes sin parsing complejo de texto
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

echo -e "${BLUE}=== TESTS PARA EL SHELL (SIN PARSING COMPLEJO) ===${NC}"
echo -e "${YELLOW}Nota: Estos tests evitan el parsing complejo según la consigna${NC}"
echo

echo -e "${YELLOW}=== TESTS BÁSICOS DE PIPES ===${NC}"

# Test 1: Comando simple sin pipes
run_test "Comando simple: ls" "ls"

# Test 2: Pipe básico simple - buscar archivos con extensión
run_test "Pipe básico: ls | grep .c" "ls | grep .c"

# Test 3: Pipe básico - buscar archivos shell
run_test "Pipe básico: ls | grep shell" "ls | grep shell"

# Test 4: Pipe básico - buscar Makefile
run_test "Pipe básico: ls | grep Makefile" "ls | grep Makefile"

# Test 5: Contar líneas con pipe
run_test "Contar con pipe: ls | wc -l" "ls | wc -l"

echo -e "${YELLOW}=== TESTS DE MÚLTIPLES PIPES ===${NC}"

# Test 6: Triple pipe - contar archivos .c
run_test "Triple pipe: ls | grep .c | wc -l" "ls | grep .c | wc -l"

# Test 7: Triple pipe - ordenar y limitar
run_test "Triple pipe: ls | sort | head -3" "ls | sort | head -3"

# Test 8: Triple pipe - buscar y ordenar
run_test "Triple pipe: ls | grep shell | sort" "ls | grep shell | sort"

echo -e "${YELLOW}=== TESTS DE COMANDOS CON ARGUMENTOS SIMPLES ===${NC}"

# Test 9: Comando con argumento simple
run_test "ls con argumento: ls -l | grep shell" "ls -l | grep shell"

# Test 10: head con número
run_test "head con número: ls | head -5" "ls | head -5"

# Test 11: wc con opción
run_test "wc con opción: ls | wc -w" "ls | wc -w"

echo -e "${YELLOW}=== TESTS DE ROBUSTEZ (SIN PARSING COMPLEJO) ===${NC}"

# Test 12: Espacios simples alrededor de pipes (sin parsing complejo)
run_test "Espacios simples: ls|grep shell" "ls|grep shell" "ls | grep shell"

# Test 13: Pipe con cat
run_test "Pipe con cat: ls | cat" "ls | cat"

# Test 14: sort simple
run_test "Sort simple: ls | sort" "ls | sort"

# Test 15: Cuádruple pipe
run_test "Cuádruple pipe: ls | grep shell | sort | head -2" "ls | grep shell | sort | head -2"

echo -e "${YELLOW}=== TESTS CON ECHO (ENTRADA CONTROLADA) ===${NC}"

# Test 16: echo simple con pipe (sin comillas complejas)
run_test "echo simple: echo test | grep test" "echo test | grep test"

# Test 17: echo con múltiples palabras (sin comillas)
run_test "echo múltiple: echo hello world | grep hello" "echo hello world | grep hello"

echo -e "${BLUE}=== TESTS EXCLUIDOS POR PARSING COMPLEJO ===${NC}"
echo -e "${YELLOW}Los siguientes tipos de comandos NO se testean según la consigna:${NC}"
echo -e "- Comandos con comillas: ls | grep \".zip\""
echo -e "- Patrones complejos: ls | grep \".png .zip\""
echo -e "- Parsing de argumentos con espacios y comillas"
echo -e "- Escape de caracteres especiales"
echo

echo -e "${BLUE}=== RESUMEN DE TESTS ===${NC}"
echo -e "Total de tests: ${TOTAL}"
echo -e "${GREEN}Pasaron: ${PASSED}${NC}"
echo -e "${RED}Fallaron: ${FAILED}${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}¡Todos los tests pasaron! ✓${NC}"
    echo -e "${BLUE}El shell maneja correctamente pipes sin parsing complejo${NC}"
    exit 0
else
    echo -e "${RED}Algunos tests fallaron. Revisa la implementación.${NC}"
    exit 1
fi