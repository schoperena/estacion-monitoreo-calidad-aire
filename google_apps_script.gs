function doGet(e) {
  return handleRequest(e);
}

function doPost(e) {
  return handleRequest(e);
}

function handleRequest(e) {
  // ID del archivo de Google Sheets (el que aparece en la URL)
  var sheetId = "1YG-QNsZTo1uTu1pK27krv6wenaQEufblueP9vAEQdKM";
  var spreadsheet = SpreadsheetApp.openById(sheetId);
  var sheet = spreadsheet.getSheetByName("Datos");

  if (!sheet) {
    return ContentService.createTextOutput("Error: hoja 'Datos' no encontrada.");
  }

  var params = e.parameter;

  // Validar parámetros
  if (!params.temperatura || !params.humedad || !params.calidad) {
    return ContentService.createTextOutput("Error: parámetros incompletos.");
  }

  // Interpretar calidad del aire
  var calidad = parseFloat(params.calidad);
  var interpretacion = "";
  if (calidad < 300) {
    interpretacion = "Aire limpio";
  } else if (calidad < 1000) {
    interpretacion = "Calidad moderada";
  } else if (calidad < 2000) {
    interpretacion = "Aire contaminado";
  } else {
    interpretacion = "Aire muy contaminado";
  }

  // Reemplazar puntos por comas
  var temperatura = parseFloat(params.temperatura).toFixed(1).replace('.', ',');
  var humedad = parseFloat(params.humedad).toFixed(1).replace('.', ',');
  var calidadTexto = calidad.toFixed(1).replace('.', ',');

  // Agregar fila
  sheet.appendRow([
    new Date(),
    temperatura,
    humedad,
    calidadTexto,
    interpretacion
  ]);

  return ContentService.createTextOutput("OK");
}
