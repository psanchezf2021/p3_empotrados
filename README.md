# p3_empotrados

Para completar esta práctica he planteado la máquina expendedora como una máquina de estados, siendo cada parte de la funcionalidad un estado según sea necesario.

Toda la funcionalidad del arranque la he incluído en el setup del programa, es decir, escribir en el lcd CARGANDO... y mediante un bucle for, encender y apagar el led 1 con delays.

Antes de entrar a la funcionalidad del servicio cabe destacar que todas las mediciones de los sensores (temperatura, humedad, distancia y joystick) se hacen en threads separados.

En el servicio se encuentra la primera máquina de estados en la cual:

- Estado 0: escribe ESPERANDO CLIENTE en el lcd hasta que la distancia sea menor que 100, entonces pasa al siguiente estado, poniendo antes una marca de tiempo con millis().
- Estado 1: primero, comprueba la marca de tiempo comparándola con el tiempo actual, y si son menos de 5 segundos, mostrará la temperatura y humedad en el lcd. Si ya han pasado 5 segundos mostrará un elemento de la lista de cafés y una lista de precios dependiendo del valor de una variable, esta variable cambia si la posición vertical del joystick es mayor que 950 o menor que 100. Si se pulsa el botón del joystick, se genera un número aleatorio entre 4000 y 8000, se pone una marca de tiempo y se pasa al estado 2.
- Estado 2: simplemente comprueba que la diferencia entre la marca de tiempo y el tiempo actual es mayor o igual al número aleatorio, si lo es, pasa al estado 3 y pone otra marca de tiempo, si no, muestra Preparando Cafe ... en el lcd y va aumentando la intensidad del led.
- Estado 3: compara la marca de tiempo para que pasen 3 segundos, muestra RETIRE BEBIDA y vuelve al estado 0.

Para entrar el estado 4 (el estado admin) he creado una interrumción asociada al botón cuando cambia el voltaje, dentro de esta, si el botón esta pulsado, pone una marca de tiempo, si el boton no está siendo pulsado, comprueba el tiempo desde que se ha pulsado hasta que se ha soltado, si es más de 5 segundos, cambia el estado a 4 o a 0 dependiendo si estaba ya en el estado 4 o no, y si es entre 2 y 3 segundos y está en el estado 0, 1, 2 o 3, vuelve al estado 0.

- Estado 4: aquí se muestran las opciones del menú de administrador igual que se mostraban las opciones de café y si el botón del joystick es pulsado, se pasará al estado 5.
- Estado 5: con un switch, dependiendo de la posición de la lista en la que se haya pulsado el botón va a mostrar una cosa u otra. Para la temperatura, humedad, tiempo y distancia simplemente se muestra en el lcd la información necesaria. Para modificar los precios es cuando la cosa cambia, primero, se mostrará la lista de los productos y cuando se seleccione uno de ellos con el botón del joystick, pasará al siguiente caso del switch, en el que según la posición del joystick se modificará el precio del producto seleccionado cambiando también la lista de precios. Si en cualquiera de los casos de este switch se mueve el joystick a la izquierda, se volverá al estado 4.

Por último he creado un watchdog para que vigile que el bucle principal ejecute cada 500ms, esto es posible ya que solo hay dos delays en todo el programa, uno en el bucle principal para que el lcd no parpadee demasiado rápido y otro en las mediciones de distancia para que el ultrasonidos funcione correctamente.

Esquema de conexiones:

![Screenshot from 2023-12-03 20-49-18](https://github.com/psanchezf2021/p3_empotrados/assets/92941198/ac9b9eb6-7afd-4bbe-86d6-592e6ff53e73)

Video del funcionamiento:

https://github.com/psanchezf2021/p3_empotrados/assets/92941198/df3069d3-dade-4d3a-876a-01a278d89722
