#' Extract the structure of a REDATAM .dicx file
#' @param dicx_file character path to the .dicx file
#' @importFrom purrr map
#' @importFrom xml2 read_xml xml_root xml_find_all xml_text
#' @export
redatam_structure <- function(dicx_file) {
  root <- xml_root(read_xml(dicx_file))

  # XML structure
  # <root>
  # ...
  # <entity>
  #  <name>REGION</name>
  #  <label>División Regional</label>
  #  <selectable>true</selectable>
  #  <refCode>IDREGION</refCode>
  #  <refLabel>NREGION</refLabel>
  #  <cmpCode>REDCODEN</cmpCode>
  #  <filename>.\CPV201701.ptr</filename>
  #  <tag>100</tag>
  #  <variable>
  #   <name>IDREGION</name>
  #   <label>Código Region</label>
  #   <varType>STRING</varType>
  #   <fieldSize>2</fieldSize>
  #   <tag>1</tag>
  #   <datasetID/>
  #   <filename>.\CPV201701.rbf</filename>
  #   <varDicChoice>
  #    <datasetType>CHR</datasetType>
  #    <datasetSize>2</datasetSize>
  #   </varDicChoice>
  # </variable>

  # convert the entities to a list
  structure <- map(
      xml_find_all(root, ".//entity"),
      ~{
      entity <- list(
        name = xml_text(xml_find_all(.x, ".//name")),
        label = xml_text(xml_find_all(.x, ".//label")),
        selectable = xml_text(xml_find_all(.x, ".//selectable")),
        refCode = xml_text(xml_find_all(.x, ".//refCode")),
        refLabel = xml_text(xml_find_all(.x, ".//refLabel")),
        cmpCode = xml_text(xml_find_all(.x, ".//cmpCode")),
        filename = xml_text(xml_find_all(.x, ".//filename")),
        tag = xml_text(xml_find_all(.x, ".//tag")),
        variable = map(xml_find_all(.x, ".//variable"), ~{
          variable <- list(
            name = xml_text(xml_find_all(.x, ".//name")),
            label = xml_text(xml_find_all(.x, ".//label")),
            varType = xml_text(xml_find_all(.x, ".//varType")),
            fieldSize = xml_text(xml_find_all(.x, ".//fieldSize")),
            tag = xml_text(xml_find_all(.x, ".//tag")),
            datasetID = xml_text(xml_find_all(.x, ".//datasetID")),
            filename = xml_text(xml_find_all(.x, ".//filename")),
            varDicChoice = list(
              datasetType = xml_text(xml_find_all(.x, ".//datasetType")),
              datasetSize = xml_text(xml_find_all(.x, ".//datasetSize"))
            )
          )
        })
      )
    })

  return(structure)
}
