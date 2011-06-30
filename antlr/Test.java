import org.antlr.runtime.*;
import org.antlr.runtime.tree.CommonTree;
import org.antlr.runtime.tree.CommonTreeNodeStream;
import org.antlr.runtime.tree.DOTTreeGenerator;

public class Test {
    public static void main(String[] args) {
	try {
		ANTLRInputStream input = new ANTLRInputStream(System.in);
		TLexer lexer = new TLexer(input);
		CommonTokenStream tokens = new CommonTokenStream(lexer);
		TParser parser = new TParser(tokens);
		TParser.root_return r = parser.root();

		CommonTree t = (CommonTree)r.getTree();
		System.out.println(t.toStringTree());
		CommonTreeNodeStream nodes = new CommonTreeNodeStream(t);
		nodes.setTokenStream(tokens);
		TWalker walker = new TWalker(nodes);
		walker.root();
	} catch (Exception e) {
		System.out.println("ee: " + e);
	}
    }
}
