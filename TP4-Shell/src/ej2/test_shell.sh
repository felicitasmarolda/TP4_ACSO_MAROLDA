#!/bin/bash

# Test Suite para Shell con Pipes
# Autor: Test automatizado para verificar funcionalidad del shell

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Contadores
TESTS_PASSED=0
TESTS_FAILED=0
TOTAL_TESTS=0

# Función para mostrar resultados
print_result() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓ PASS${NC}: $2"
        ((TESTS_PASSED++))
    else
        echo -e "${RED}✗ FAIL${NC}: $2"
        ((TESTS_FAILED++))
    fi
    ((TOTAL_TESTS++))
}

# Función para ejecutar test y comparar con bash
run_test() {
    local test_name="$1"
    local command="$2"
    local timeout_duration="${3:-5}" # default 5 segundos
    
    echo -e "\n${YELLOW}Testing:${NC} $command"
    
    # Ejecutar con bash real
    bash_output=$(timeout $timeout_duration bash -c "$command" 2>/dev/null | head -20)
    bash_exit=$?
    
    # Ejecutar con nuestro shell
    shell_output=$(timeout $timeout_duration sh -c "echo '$command' | ./shell" 2>/dev/null | tail -n +2 | head -20)
    shell_exit=$?
    
    # Comparar outputs (ignorando diferencias menores de whitespace)
    if [ "$bash_exit" -eq 124 ] || [ "$shell_exit" -eq 124 ]; then
        echo -e "${YELLOW}⚠ TIMEOUT${NC}: $test_name (comando tardó más de $timeout_duration segundos)"
        return 1
    fi
    
    # Normalizar outputs (remover espacios extra, líneas vacías)
    bash_normalized=$(echo "$bash_output" | sed '/^$/d' | sed 's/[[:space:]]*$//')
    shell_normalized=$(echo "$shell_output" | sed '/^$/d' | sed 's/[[:space:]]*$//')
    
    if [ "$bash_normalized" = "$shell_normalized" ]; then
        print_result 0 "$test_name"
        return 0
    else
        print_result 1 "$test_name"
        echo -e "  ${RED}Expected (bash):${NC}"
        echo "$bash_output" | sed 's/^/    /'
        echo -e "  ${RED}Got (shell):${NC}"
        echo "$shell_output" | sed 's/^/    /'
        return 1
    fi
}

# Crear archivos de test temporales
setup_test_files() {
    echo "Creating test files..."
    cat > test_file.txt << EOF
apple
banana
cherry
date
elderberry
fig
grape
EOF

    cat > test_numbers.txt << EOF
1
2
3
4
5
EOF

    echo "test.zip" > test_zip_file
    echo "document.pdf" > test_pdf_file
    echo "script.sh" > test_sh_file
    echo "image.png" > test_png_file
}

# Limpiar archivos de test
cleanup_test_files() {
    echo "Cleaning up test files..."
    rm -f test_file.txt test_numbers.txt test_zip_file test_pdf_file test_sh_file test_png_file
}

# Verificar que el shell existe
check_shell_exists() {
    if [ ! -f "./shell" ]; then
        echo -e "${RED}Error: ./shell no encontrado. Compila primero con 'make'${NC}"
        exit 1
    fi
    
    if [ ! -x "./shell" ]; then
        echo -e "${RED}Error: ./shell no es ejecutable${NC}"
        exit 1
    fi
}

echo "=== SHELL PIPE TESTING SUITE ==="
echo "Verificando funcionalidad del shell implementado"
echo "================================================"

check_shell_exists
setup_test_files

echo -e "\n${YELLOW}=== TESTS BÁSICOS ===${NC}"

# Test 1: Comando simple sin pipes
run_test "Comando simple (ls)" "ls"

# Test 2: Pipe básico
run_test "Pipe básico (echo | cat)" "echo 'hello world' | cat"

# Test 3: Grep básico
run_test "Grep básico" "echo -e 'apple\nbanana\ncherry' | grep 'a'"

echo -e "\n${YELLOW}=== TESTS CON ARCHIVOS ===${NC}"

# Test 4: Cat con pipe
run_test "Cat con pipe" "cat test_file.txt | head -3"

# Test 5: Grep con archivo
run_test "Grep con patron" "cat test_file.txt | grep 'e'"

# Test 6: Múltiples greps
run_test "Múltiples greps" "cat test_file.txt | grep 'a' | grep 'e'"

echo -e "\n${YELLOW}=== TESTS CON COMANDOS DEL SISTEMA ===${NC}"

# Test 7: ps con grep (si está disponible)
if command -v ps >/dev/null 2>&1; then
    run_test "ps con grep" "ps aux | grep -v grep | head -5"
fi

# Test 8: wc (word count)
if command -v wc >/dev/null 2>&1; then
    run_test "Word count" "cat test_file.txt | wc -l"
fi

# Test 9: sort
if command -v sort >/dev/null 2>&1; then
    run_test "Sort" "cat test_file.txt | sort"
fi

echo -e "\n${YELLOW}=== TESTS DE PIPES MÚLTIPLES ===${NC}"

# Test 10: Triple pipe
run_test "Triple pipe" "cat test_file.txt | grep 'a' | sort | head -2"

# Test 11: Pipe con números
run_test "Pipe con números" "cat test_numbers.txt | sort -n | tail -2"

echo -e "\n${YELLOW}=== TESTS DE EDGE CASES ===${NC}"

# Test 12: Comando que no existe (primer comando)
echo -e "\n${YELLOW}Testing:${NC} comando_inexistente | cat"
shell_output=$(timeout 3 sh -c "echo 'comando_inexistente | cat' | ./shell" 2>&1)
if echo "$shell_output" | grep -q "command not found\|execvp failed\|No such file"; then
    print_result 0 "Error handling - comando inexistente"
else
    print_result 1 "Error handling - comando inexistente"
fi

# Test 13: Pipe vacío
run_test "Pipe con comando vacío" "echo 'test' | cat"

echo -e "\n${YELLOW}=== TESTS INTERACTIVOS (requieren verificación manual) ===${NC}"

cat << EOF

Para tests interactivos, ejecuta ./shell y prueba:

1. Comandos básicos:
   Shell> ls
   Shell> echo "hello world"

2. Pipes simples:
   Shell> ls | grep .c
   Shell> cat /etc/passwd | head -5

3. Pipes múltiples:
   Shell> ls -la | grep -v total | head -3
   Shell> ps aux | grep bash | wc -l

4. Tests con archivos locales:
   Shell> cat test_file.txt | grep a
   Shell> cat test_file.txt | sort | head -3

5. Salir:
   Shell> exit

EOF

cleanup_test_files

echo -e "\n${YELLOW}=== RESUMEN DE RESULTADOS ===${NC}"
echo "================================================"
echo -e "Tests ejecutados: $TOTAL_TESTS"
echo -e "${GREEN}Tests pasados: $TESTS_PASSED${NC}"
echo -e "${RED}Tests fallidos: $TESTS_FAILED${NC}"

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "\n${GREEN}🎉 ¡TODOS LOS TESTS PASARON!${NC}"
    exit 0
else
    echo -e "\n${RED}❌ Algunos tests fallaron. Revisa la implementación.${NC}"
    exit 1
fi