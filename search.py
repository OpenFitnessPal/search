#!/bin/python3
import myfitnesspal
import sys

c = myfitnesspal.Client()

args = sys.argv[1:]
args = ' '.join(args)
args = args.split('?')

food = args[0]
queries = {}
if len(args) > 1:
    query_str = args[1]
    query_arr = query_str.split("&")

    for query in query_arr:
        split = query.split("=")

        if len(split) == 0:
            continue

        key = split[0]

        if len(split) == 1:
            queries[key] = True
        else:
            value = split[1]
            queries[key] = value

results = c.get_food_search_results(food)

data = []

def process_nutrients(f):
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

    return d

def process_servings(f):
    servings = []

    for s in f.servings:
        se = {}
        se["mult"] = s.nutrition_multiplier
        se["value"] = s.value
        se["unit"] = s.unit.replace("'", "`").replace('"', "`")

        servings.append(se)

    return servings

#avoid_<nutrient>=<threshold>
def filter_nutrients(f):
    for avoid in [q for q in list(queries) if q.startswith('avoid')]:
        n = '_'.join(avoid.split("_")[1:])
        if n not in f:
            continue

        threshold = queries[avoid]
        if threshold == True:
            threshold = 0.1
        else:
            try:
                threshold = float(threshold)
            except:
                threshold = 0.1

        if threshold < 0.0:
            threshold = 0.0

        if f[n] > threshold:
            return False
    return True

def filter(f):
    good = True
    good = filter_nutrients(f)
    return good

def sort_generics(item):
    if 'generics' in queries:
        return item['brand'] == 'Generic' or item['brand'] == ''

def sort():
    data.sort(reverse=True, key=sort_generics)

i = 0
idx = 0
while i < 10:
    f = results[idx]
    idx += 1

    d = process_nutrients(f)
    if not filter(d):
        continue

    d["servings"] = process_servings(f)

    data.append(d)
    i += 1

sort()

print(str(data).replace("'", '"').replace(u'\\xa0', u' '))
