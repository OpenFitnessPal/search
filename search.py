#!/bin/python3
import myfitnesspal
import sys

c = myfitnesspal.Client()

results = c.get_food_search_results(' '.join(sys.argv[1:]))

data = []

for f in results[:10]:
    d = {}
    def field(name, field, default):
        try:
            attr = getattr(f, field)
            if isinstance(attr, str):
                attr = attr.replace("'", "`")
                attr = attr.replace('"', "`")
                if attr == "":
                    attr = default
            d[name] = attr
        except Exception as e:
            d[name] = default;
            pass

    def field_same_name(name, default):
        field(name, name, default)

    field_same_name("brand", "Generic")
    field_same_name("name", "Unknown")
    field("id", "mfp_id", "0")

    field_same_name("calcium", 0)
    field_same_name("cholesterol", 0)
    field_same_name("fat", 0)
    field_same_name("fiber", 0)
    field_same_name("iron", 0)
    field_same_name("protein", 0)
    field_same_name("potassium", 0)
    field_same_name("sodium", 0)
    field_same_name("sugar", 0)

    field("sat", "saturated_fat", 0)
    field("mono", "monounsaturated_fat", 0)
    field("poly", "polyunsaturated_fat", 0)
    field("trans", "trans_fat", 0)
    field("carbs", "carbohydrates", 0)
    field("vitaminA", "vitamin_a", 0)
    field("vitaminC", "vitamin_c", 0)

    servings = []
    for s in f.servings:
        se = {}
        se["mult"] = s.nutrition_multiplier
        se["value"] = s.value
        se["unit"] = s.unit.replace("'", "`").replace('"', "`")

        servings.append(se)

    d["servings"] = servings

    data.append(d)

print(str(data).replace("'", '"').replace(u'\\xa0', u' '))
