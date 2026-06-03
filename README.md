# Compiladores 1 Q2 2026
## Por Johnny Melgar; Supervisado por Ing. Ivan Deras
### Estructura del Proyecto
La estructura no estan complicada, siendo la siguiente:
```structure
Proyecto
|
|-examples 
    |-boolean.txt
    |-fibonacci.txt
    |-<archivos de prueba>
|
|-include
    |-lexer.hpp
    |-parser.hpp
    |-<archivos de encabezados>
|
|-src
    |-lexer.cpp
    |-parser.cpp
    |-<archivo del cuerpo del código>
|
|-main.cpp 
|-.gitignore
|-lexer.l
|-lexer.dot.gv
|
|- build
    |-Gol (ejecutable)
```

### Resumen
Se nos asigno crear un compilador de Go a una escala pequeña en donde acepte solo int y bool como funciones.
El proyecto consta en este momento de 3 partes:
- * Main.cpp * (método principal del programa que permite seleccionar los archivos mediante parametros)
- * Lexer.pp * (tokeniza lo que se le manda al programa)
- * Parser.cpp* (Hace una verificación sintáctica del archivo mandado al programa)

En general, es lo básico para ver si el archivo tiene escrito algo permitido a nivel de su gramatica (Aunque falte el analizador semántico)

### ¿Cómo ejecutarlo?
Use los siguientes comandos: 
```terminal
cmake -B <nombre_del_constructor> -S <carpeta_del_proyecto>
cmake --build <nombre_del_constructor>
```
Esos comandos le permiten crear el ejecutable, para usarlo debera solo debera usar el siguiente comando en terminal:
```terminal
<nombre_del_constructor>/Gol <archivo_con_código>
```

Si no se manda el archivo con código se quejara el programa, siempre debe enviarlo.

### ¿Cómo funciona?
Realmente todo el compilador funciona mediante a características bien específicas siendo:
- Análizador léxico (lexer.hpp/.cpp)
Genera tokens en base a como esta establecido la detección de tokens del lexer.l y sigue la lógica del automata que aparece en lexer.dot.gv. 
- Análizador sintactico (parser.hpp/.cpp)
En base a la gramática, verifica si los tokens dados por el análizador léxico esten en un orden congruente.
- Análizador semántico e Interprete (Uso del árbol de sintaxis abstracta)
Realiza una supervisión de la gramática dada e intenta cargar las acciones dadas del archivo proveído.

