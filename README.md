# Belkan Pipeline Agents — Operación Contención Belkanita

Solución a la Práctica 2 de la asignatura **Inteligencia Artificial**
(E.T.S. de Ingenierías Informática y de Telecomunicación, Universidad de Granada),
curso 2025-2026.

Dos agentes autónomos —un **Ingeniero** y su **Técnico**— deben cooperar para
localizar una filtración tóxica en un entorno simulado y construir una red de
tuberías que la conduzca hasta una planta de tratamiento, combinando
comportamientos **reactivos** (exploración de un mapa desconocido) y
**deliberativos** (búsqueda de rutas óptimas y planificación de la red con A*).

## Créditos

- **Enunciado y código base** (motor de simulación, clases `Comportamiento`,
  interfaz gráfica) proporcionados por el
  [Departamento de Ciencias de la Computación e Inteligencia Artificial](https://decsai.ugr.es/),
  Universidad de Granada, como parte de la asignatura de Inteligencia Artificial.
  El repositorio oficial con el simulador completo está en
  [github.com/ugr-ccia-IA/practica2](https://github.com/ugr-ccia-IA/practica2).
- Este repositorio contiene **únicamente los archivos de solución** entregados
  como práctica: `ingeniero.hpp`, `ingeniero.cpp`, `tecnico.hpp`, `tecnico.cpp`.

## Niveles completados

| Nivel | Nombre | Tipo | Descripción |
|---|---|---|---|
| 0 | Rastreadores de Alcantarillas | Reactivo | Ambos agentes llegan a una planta de tratamiento por caminos conocidos, en mapa desconocido |
| 1 | De Reconocimiento por la Naturaleza | Reactivo | Exploración y mapeo de caminos y senderos del territorio |
| 2 | Corre, Ingeniero, Corre! | Deliberativo (búsqueda) | Ruta más rápida del Ingeniero hasta la Belkanita, minimizando instantes de simulación |
| 3 | El Técnico mide sus Esfuerzos | Deliberativo (A*) | Ruta de menor consumo de energía del Técnico, con algoritmo A* |
| 4 | Planifica, Ingeniero, Planifica! | Deliberativo | Planificación de la red de tuberías con el menor número de tramos posible |
| 5 | A poner Tuberías | Mixto | Construcción real de la red combinando exploración, rutas y planificación, con mapa conocido |
| 6 | ¿Qué alguien ponga luz? | Mixto | Igual que el nivel 5, pero con el mapa inicialmente desconocido |

## Cómo compilar y ejecutar

Este repositorio no incluye el simulador completo (motor gráfico, mapas, clases
base), ya que es material propiedad del departamento. Para probar esta solución:

1. Clona el repositorio base del simulador:
```bash
   git clone https://github.com/ugr-ccia-IA/practica2.git
```
2. Sigue las instrucciones de instalación de ese repositorio (`install.sh`),
   que instala las dependencias necesarias:
   `freeglut3-dev libjpeg-dev libopenmpi-dev openmpi-bin openmpi-doc
   libxmu-dev libxi-dev cmake libboost-all-dev`.
3. Sustituye los archivos `ingeniero.hpp`, `ingeniero.cpp`, `tecnico.hpp` y
   `tecnico.cpp` de la carpeta `Comportamientos_Agentes` por los de este
   repositorio.
4. Compila con `make`. Se generan dos ejecutables:
   - `./practica2` — versión con interfaz gráfica.
   - `./practica2SG` — versión en modo *batch*, sin interfaz (útil para debug).

### Ejemplo de ejecución en modo batch

```bash
./practica2SG -m ./mapas/mapa30.map -seed 1 -n 0 -i 24 12 4 -t 9 4 2
```

Esto carga `mapa30.map`, nivel 0, con el Ingeniero en fila 24, columna 12,
orientación sur, y el Técnico en fila 9, columna 4, orientación este.

Parámetros disponibles: `-m` (mapa), `-seed`, `-n` (nivel 0-6), `-i` (posición
y orientación del Ingeniero), `-t` (posición y orientación del Técnico), `-O`
(posición de la filtración), `-Tiempo`, `-Energia`, `-Ambiental` (impacto
ecológico máximo).

## Estructura del código

- `ingeniero.hpp` / `ingeniero.cpp`: comportamiento del agente Ingeniero
  (exploración reactiva, búsqueda de rutas, planificación de tuberías, JUMP,
  RAISE, DIG).
- `tecnico.hpp` / `tecnico.cpp`: comportamiento del agente Técnico
  (exploración reactiva, rutas de mínima energía con A*, instalación de
  tuberías coordinada con el Ingeniero).

## Autor

Ignacio Martínez — 3º de Ingeniería Informática, UGR.
